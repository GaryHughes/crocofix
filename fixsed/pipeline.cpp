#include "pipeline.hpp"
#include <iostream>
#include <sstream>
#include <libcrocofix/message.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

using boost::asio::ip::tcp;

const char* fix_message_prefix = "8=FIX";

pipeline::pipeline(const options& options)
:   m_options(options)
{
}

void pipeline::process_messages(
    tcp::socket& read_socket, 
    tcp::socket& write_socket, 
    const std::string& read_label,
    const std::string& write_label)
{
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

                if (m_options.pretty_print()) {
                    message.pretty_print(std::cout);
                }
                else {
                    for (const auto& field : message.fields()) {
                        std::cout << field.tag() << "=" << field.value();
                    }
                }
                std::cout << '\n';
            
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
        std::cerr << "exception in thread: " << e.what() << "\n";
    }
}

void close_socket(tcp::socket& socket)
{
    try
    {
        if (socket.is_open()) {
            socket.close();
        }
    }
    catch (std::exception& ex)
    {
        std::cerr << "Error closing socket: " << ex.what() << std::endl;
    }
}

void pipeline::run()
{
    try
    {
        boost::asio::io_context io_context;
        
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), m_options.in_port()));

        // TODO - bind host

        acceptor.listen();
        tcp::socket initiator_socket(io_context);

        std::ostringstream msg;
			  msg << "waiting for initiator [" << (m_options.in_host() ? *m_options.in_host() : "*") << ":" << m_options.in_port() << "]";
			  std::cout << msg.str() << std::endl;

        acceptor.accept(initiator_socket);

        msg.str("");
			  msg << "accepted initiator [" 
            << initiator_socket.remote_endpoint().address().to_string() << ":"
            << initiator_socket.remote_endpoint().port() << "]";
			  std::cout << msg.str() << std::endl;

        msg.str("");
        msg << "resolving acceptor [" << m_options.out_host() << ":" << m_options.out_port() << "]";
        std::cout << msg.str() << std::endl;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(m_options.out_host(), std::to_string(m_options.out_port()));
        
        msg.str("");
        msg << "connecting to acceptor [" << m_options.out_host() << ":" << m_options.out_port() << "]";
        std::cout << msg.str() << std::endl;
  
        tcp::socket acceptor_socket(io_context);

        // TODO - bind

        boost::asio::connect(acceptor_socket, endpoints);

        msg.str("");
			  msg << "connected to acceptor [" 
            << acceptor_socket.remote_endpoint().address().to_string() << ":"
            << acceptor_socket.remote_endpoint().port() << "]";
			  std::cout << msg.str() << std::endl;

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
        std::cerr << "error: " << ex.what() << std::endl;
    }
}