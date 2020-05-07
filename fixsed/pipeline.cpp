#include "pipeline.hpp"
#include <iostream>
#include <sstream>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

using boost::asio::ip::tcp;

const char* fix_message_prefix = "8=FIX";

pipeline::pipeline(const options& options)
:   m_options(options)
{
}

void pipeline::log_message(boost::log::sources::severity_logger<boost::log::trivial::severity_level>& logger, const crocofix::message& message)
{
    std::ostringstream msg;

    if (m_options.pretty_print()) {
        message.pretty_print(msg);
    }
    else {
        for (const auto& field : message.fields()) {
            msg << field.tag() << "=" << field.value() << '\01';
        }
    }

    log_info(logger) << msg.str();
}

void pipeline::process_messages(
    tcp::socket& read_socket, 
    tcp::socket& write_socket, 
    const std::string& read_label,
    const std::string& write_label)
{
    boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger;

    try
    {
        auto read_buffer = std::array<char, 8192>();
        auto write_buffer = std::array<char, 8192>();

        size_t write_offset{0};
        size_t read_offset{0};

        crocofix::message message;

        while (true)
        {
            if (write_offset == read_buffer.size())  {
                // The buffer is full.
                auto remainder_size = write_offset - read_offset;
                if (remainder_size > 0) {
                    // We have some left over data, copy the left overs back to the start of the buffer
                    // so we have room to read more bytes. Due to the way message::decode works this can
                    // only be less than the length of one tag/value pair.
                    memcpy(&read_buffer[0], &read_buffer[read_offset], remainder_size);
                }
                read_offset = 0;
                write_offset = remainder_size;
            }

            // This is either our first read at the start of the buffer or we have previously
            // performed a partial read that did not contain a complete message.
            auto buffer = boost::asio::buffer(&*read_buffer.begin() + write_offset, 
                                              read_buffer.size() - write_offset);

            boost::system::error_code error;
          
            size_t bytes_read = read_socket.read_some(buffer, error);
          
            if (error == boost::asio::error::eof) {
                break;
            }
            
            if (error) {
                throw boost::system::system_error(error);
            }

            write_offset += bytes_read;
    
            while (true) 
            {
                auto [consumed, complete] = message.decode(std::string_view(&read_buffer[read_offset], write_offset - read_offset));

                read_offset += consumed;

                if (!complete) {
                    break;
                }

                log_message(logger, message);
            
                auto encoded_size = message.encode(gsl::span(&write_buffer[0], write_buffer.size()));

                if (encoded_size == 0) {
                    throw std::runtime_error("failed to encode message");
                }

                auto buffer = boost::asio::buffer(&*write_buffer.begin(), encoded_size);

                write_socket.write_some(buffer);

                message.fields().clear();
            }
        }
    }
    catch (std::exception& e)
    {
        log_error(logger) << "exception in thread: " << e.what();
    }
}

void pipeline::close_socket(tcp::socket& socket)
{
    boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger;

    try
    {
        if (socket.is_open()) {
            socket.close();
        }
    }
    catch (std::exception& ex)
    {
        log_error(logger) << "Error closing socket: " << ex.what();
    }
}

void pipeline::run()
{
    boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger;

    try
    {
        
        boost::asio::io_context io_context;
        
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), m_options.in_port()));

        // TODO - bind host

        acceptor.listen();
        tcp::socket initiator_socket(io_context);

	    log_info(logger) << "waiting for initiator [" << (m_options.in_host() ? *m_options.in_host() : "*") << ":" << m_options.in_port() << "]";

        acceptor.accept(initiator_socket);

        log_info(logger) << "accepted initiator [" 
            << initiator_socket.remote_endpoint().address().to_string() << ":"
            << initiator_socket.remote_endpoint().port() << "]";

        log_info(logger) << "resolving acceptor [" << m_options.out_host() << ":" << m_options.out_port() << "]";

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(m_options.out_host(), std::to_string(m_options.out_port()));
        
        log_info(logger) << "connecting to acceptor [" << m_options.out_host() << ":" << m_options.out_port() << "]";
  
        tcp::socket acceptor_socket(io_context);

        // TODO - bind

        boost::asio::connect(acceptor_socket, endpoints);

	    log_info(logger) << "connected to acceptor [" 
            << acceptor_socket.remote_endpoint().address().to_string() << ":"
            << acceptor_socket.remote_endpoint().port() << "]";

        std::thread thread([&] {
            process_messages(acceptor_socket, initiator_socket, "ACCEPTOR", "INIITIATOR");
            close_socket(initiator_socket);
            close_socket(acceptor_socket);
        });

        process_messages(initiator_socket, acceptor_socket, "INITIATOR", "ACCEPTOR");
        close_socket(initiator_socket);
        close_socket(acceptor_socket);

        thread.join();
    }
    catch(std::exception& ex)
    {
        log_error(logger) << "error: " << ex.what();
    }
}