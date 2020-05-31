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
        int port = 5000;

        boost::asio::io_context io_context;
        crocofix::boost_asio_scheduler scheduler(io_context);
        
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

        std::cout << "waiting for initiator [*:" << port << "]" << std::endl;

        tcp::socket socket(io_context);
        
        acceptor.listen();
        acceptor.accept(socket);

        std::cout << "accepted initiator [" 
            << socket.remote_endpoint().address().to_string() << ":"
            << socket.remote_endpoint().port() << "]" << std::endl;
  
        crocofix::socket_reader reader(socket);
        crocofix::socket_writer writer(socket);
        
        crocofix::session session(reader, writer, scheduler, crocofix::FIX_5_0SP2::orchestration());

        session.logon_behaviour(crocofix::behaviour::acceptor);
        session.begin_string("FIX.5.0SP2");
        session.sender_comp_id("ACCEPTOR");
        session.target_comp_id("INITIATOR");

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