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

void process_new_order_single(crocofix::session& session, const crocofix::message& /*message*/, boost::asio::io_context& /*io_context*/)
{
    auto execution_report = crocofix::message(true, {
        crocofix::field( crocofix::FIX_5_0SP2::field::MsgType::Tag, crocofix::FIX_5_0SP2::message::ExecutionReport::MsgType ),
    });

    session.send(execution_report);
}

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        const int port = 5000;

        boost::signals2::connection message_received_connection;
        boost::signals2::connection message_sent_connection;
       
        boost::asio::io_context io_context;
        crocofix::boost_asio_scheduler scheduler(io_context);
        
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

        std::cout << "waiting for initiator [*:" << port << "]" << std::endl; // NOLINT(performance-avoid-endl)

        tcp::socket socket(io_context); // NOLINT(clang-analyzer-optin.cplusplus.UninitializedObject)
        
        acceptor.listen();
        acceptor.accept(socket);

        std::cout << "accepted initiator [" 
            << socket.remote_endpoint().address().to_string() << ":"
            << socket.remote_endpoint().port() << "]" << std::endl; // NOLINT(performance-avoid-endl)
  
        crocofix::socket_reader reader(socket);
        crocofix::socket_writer writer(socket);
        
        crocofix::session session(reader, writer, scheduler, crocofix::FIX_5_0SP2::orchestration());

        session.logon_behaviour(crocofix::behaviour::acceptor);
        session.begin_string("FIXT.1.1");
        session.sender_comp_id("ACCEPTOR");
        session.target_comp_id("INITIATOR");

        auto information_connection = session.information.connect([&](const auto& message) { std::cout << "INFO  " << message << std::endl; });
        auto warning_connection = session.warning.connect([&](const auto& message) { std::cout << "WARN  " << message << std::endl; });
        auto error_connection = session.error.connect([&](const auto& message) { std::cout << "ERROR " << message << std::endl; });

        message_received_connection = session.message_received.connect([&](const auto& message) {
            //std::cout << "IN " << message.MsgType() << '\n';
            //message.pretty_print(std::cout);
            if (message.MsgType() == crocofix::FIX_5_0SP2::message::NewOrderSingle::MsgType) {
                process_new_order_single(session, message, io_context);
            }
        });

        message_sent_connection = session.message_sent.connect([&](const auto& message) {
            //std::cout << "OUT " << message.MsgType() << '\n';
            //message.pretty_print(std::cout);
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