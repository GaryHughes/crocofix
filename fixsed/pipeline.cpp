#include "pipeline.hpp"
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>
#include <libcrocofix/message.hpp>

using boost::asio::ip::tcp;

const char* fix_message_prefix = "8=FIX";

void process_messages(tcp::socket& read_socket, 
                      tcp::socket& write_socket, 
                      const std::string& read_label,
                      const std::string& write_label)
{
    try
    {
        auto data = std::array<char, 8192>();

        size_t write_offset{0};
        size_t read_offset{0};

        while (true)
        {
            if (write_offset == data.size())  {
                // The buffer is full.
                auto remainder_size = write_offset - read_offset;
                if (remainder_size > 0) {
                    // We have some left over data, copy the left overs back to the start of the buffer
                    // so we have room to read more bytes. Due to the way message::decode works this can
                    // only be less than the length of one tag/value pair.
                    memcpy(&data[0], &data[read_offset], remainder_size);
                }
                read_offset = 0;
                write_offset = remainder_size;
            }

            // This is either our first read at the start of the buffer or we have previously
            // performed a partial read that did not contain a complete message.
            auto buffer = boost::asio::buffer(&*data.begin() + write_offset, 
                                              data.size() - write_offset);

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
                crocofix::message message;

                auto [consumed, complete] = message.decode(std::string_view(&data[read_offset], write_offset - read_offset));

                read_offset += consumed;

                if (!complete) {
                    break;
                }

                for (const auto& field : message.fields()) {
                    std::cout << field.tag() << " - " << field.value() << std::endl;
                }

                // TODO - write
                std::cout << read_label << " -> " << write_label << " : " << message.MsgType() << std::endl;

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

void pipeline::run(const options& options)
{
    try
    {
        boost::asio::io_context io_context;
        
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), options.in_port()));

        // TODO - bind host

        acceptor.listen();
        tcp::socket initiator_socket(io_context);

        std::ostringstream msg;
			  msg << "waiting for initiator [" << (options.in_host() ? *options.in_host() : "*") << ":" << options.in_port() << "]";
			  std::cout << msg.str() << std::endl;

        acceptor.accept(initiator_socket);

        msg.str("");
			  msg << "accepted initiator [" 
            << initiator_socket.remote_endpoint().address().to_string() << ":"
            << initiator_socket.remote_endpoint().port() << "]";
			  std::cout << msg.str() << std::endl;

        msg.str("");
        msg << "resolving acceptor [" << options.out_host() << ":" << options.out_port() << "]";
        std::cout << msg.str() << std::endl;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(options.out_host(), std::to_string(options.out_port()));
        
        msg.str("");
        msg << "connecting to acceptor [" << options.out_host() << ":" << options.out_port() << "]";
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