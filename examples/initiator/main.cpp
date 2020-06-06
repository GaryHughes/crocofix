#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <libcrocofix/session.hpp>
#include <libcrocofix/socket_reader.hpp>
#include <libcrocofix/socket_writer.hpp>
#include <libcrocofix/boost_asio_scheduler.hpp>
#include <libcrocofixdictionary/fix50SP2_orchestration.hpp>
#include <libcrocofixdictionary/fix50SP2_messages.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

using boost::asio::ip::tcp;

void generate_orders(crocofix::session& session)
{
    for (size_t count = 0; count < 10; ++count)
    {
        auto new_order_single = crocofix::message(true, {
        { 
            crocofix::FIX_5_0SP2::field::MsgType::Tag, crocofix::FIX_5_0SP2::message::NewOrderSingle::MsgType },
        });

        session.send(new_order_single);    
    }
}

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
        session.begin_string("FIXT.1.1");
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

        session.state_changed.connect([&](auto previous, auto current) {
            if (current == crocofix::session_state::logged_on) {
                generate_orders(session);    
            }
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