#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <libcrocofix/session.hpp>
#include <libcrocofix/socket_reader.hpp>
#include <libcrocofix/socket_writer.hpp>
#include <libcrocofix/boost_asio_scheduler.hpp>
#include <libcrocofixdictionary/fix50SP2_orchestration.hpp>

using boost::asio::ip::tcp;

int main(int, char**)
{
    try
    {
        const std::string host = "127.0.0.1";
        int port = 5000;

        boost::asio::io_context io_context;
        crocofix::boost_asio_scheduler scheduler(io_context);

        std::cout << "connecting to acceptor [" << host << ":" << port << "]" << std::endl;

        tcp::socket socket(io_context);

        tcp::endpoint endpoint(boost::asio::ip::address::from_string(host), port);
        socket.connect(endpoint);

        crocofix::socket_reader reader(socket);
        crocofix::socket_writer writer(socket);
        
        crocofix::session session(reader, writer, scheduler, crocofix::FIX_5_0SP2::orchestration());

        session.logon_behaviour(crocofix::behaviour::initiator);
        session.test_request_delay(2000);
        session.begin_string("FIX.5.0SP2");
        session.sender_comp_id("INITIATOR");
        session.target_comp_id("ACCEPTOR");

        session.information.connect([&](const auto& message) { std::cout << "INFO  " << message << std::endl; });
        session.warning.connect([&](const auto& message) { std::cout << "WARN  " << message << std::endl; });
        session.error.connect([&](const auto& message) { std::cout << "ERROR " << message << std::endl; });

        session.message_received.connect([&](const auto& message) {
            std::cout << "IN  ";
            message.pretty_print(std::cout);
        });

        session.message_sent.connect([&](const auto& message) {
            std::cout << "OUT ";
            message.pretty_print(std::cout);
        });

        session.open();
        scheduler.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}