import crocofix;

#include <exception>
#include <iostream>
#include <print>
#include <boost/asio.hpp>
#include <boost/signals2/connection.hpp>
#include <libcrocofixdictionary/fix50SP2_orchestration.hpp>
#include <libcrocofixdictionary/fix50SP2_messages.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

using boost::asio::ip::tcp;

namespace {

void generate_orders(crocofix::session& session, size_t number_of_orders_to_send)
{
    for (size_t count = 0; count < number_of_orders_to_send; ++count)
    {
        auto new_order_single = crocofix::message(true, {
            crocofix::field( crocofix::FIX_5_0SP2::field::MsgType::Tag, crocofix::FIX_5_0SP2::message::NewOrderSingle::MsgType ),
        });

        session.send(new_order_single);

        if (count % 10000 == 0) {
            std::println("count  {}", count);
        }    
    }
}

}

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        const std::string host = "127.0.0.1";
        const int port = 8089;
        size_t number_of_orders_to_send = 100000;

        boost::signals2::connection message_received_connection;
        boost::signals2::connection message_sent_connection;
        boost::signals2::connection state_changed_connection;

        boost::asio::io_context io_context;
        crocofix::boost_asio_scheduler scheduler(io_context);

        std::println("connecting to acceptor [{}:{}]", host, port); 

        tcp::socket socket(io_context); // NOLINT(clang-analyzer-optin.cplusplus.UninitializedObject)

        const tcp::endpoint endpoint(boost::asio::ip::make_address(host), port);
        socket.connect(endpoint);

        crocofix::socket_reader reader(socket);
        crocofix::socket_writer writer(socket);
        
        crocofix::session session(reader, writer, scheduler, crocofix::FIX_5_0SP2::orchestration());

        session.logon_behaviour(crocofix::behaviour::initiator);
        session.test_request_delay(2000);
        session.begin_string("FIXT.1.1");
        session.sender_comp_id("INITIATOR");
        session.target_comp_id("ACCEPTOR");

        auto information_connection = session.information.connect([&](const auto& message) { std::println("INFO   {}", message); });
        auto warning_connection = session.warning.connect([&](const auto& message) { std::println("WARN   {}", message); });
        auto error_connection = session.error.connect([&](const auto& message) { std::println("ERROR  {}", message); });

        size_t acks_received = 0;

        message_received_connection = session.message_received.connect([&](const auto& message) {
            //std::println("IN   {}", message.MsgType());
            //message.pretty_print(std::cout);
            if (message.MsgType() != crocofix::FIX_5_0SP2::message::ExecutionReport::MsgType) {
                return;
            }

            if (++acks_received >= number_of_orders_to_send) {
                io_context.stop();
            }
        });

        message_sent_connection = session.message_sent.connect([&](const auto& message) {
            //std::println("OUT  {}", message.MsgType());
            //message.pretty_print(std::cout);
        });

        state_changed_connection = session.state_changed.connect([&](auto /*previous*/, auto current) {
            if (current == crocofix::session_state::logged_on) {
                scheduler.schedule([&]() {
                    generate_orders(session, number_of_orders_to_send);
                });
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