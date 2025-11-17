import crocofix;
import crocofix.dictionary;

#include <iostream>
#include <print>
#include <boost/asio.hpp>
#include <boost/signals2/connection.hpp>

using boost::asio::ip::tcp;

namespace {

void process_new_order_single(crocofix::session& session, const crocofix::message& /*message*/, boost::asio::io_context& /*io_context*/)
{
    auto execution_report = crocofix::message(true, {
        crocofix::field( crocofix::FIX_5_0SP2::field::MsgType::Tag, crocofix::FIX_5_0SP2::message::ExecutionReport::MsgType ),
    });

    session.send(execution_report);
}

}

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        for (;;)
        {
     
        const int port = 8089;

        boost::signals2::connection message_received_connection;
        boost::signals2::connection message_sent_connection;
       
        boost::asio::io_context io_context;
        crocofix::boost_asio_scheduler scheduler(io_context);
        
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

        std::println("waiting for initiator [*:{}]", port);
        std::println("listening...");

        acceptor.listen();
    
            try
            {
                tcp::socket socket(io_context); // NOLINT(clang-analyzer-optin.cplusplus.UninitializedObject)
                
                std::println("accepting...");

                acceptor.accept(socket);

                std::println("accepted initiator [{}:{}]", 
                             socket.remote_endpoint().address().to_string(),
                             socket.remote_endpoint().port());

                crocofix::socket_reader reader(socket);
                crocofix::socket_writer writer(socket);

               // auto reader_read_connection = reader.message_read.connect([&](const auto&) { std::cout << "READER.READ\n"; });
                
                crocofix::session session(reader, writer, scheduler, crocofix::FIX_5_0SP2::orchestration());

                session.logon_behaviour(crocofix::behaviour::acceptor);
                session.begin_string("FIXT.1.1");
                session.sender_comp_id("ACCEPTOR");
                session.target_comp_id("INITIATOR");

                auto information_connection = session.information.connect([&](const auto& message) { std::println("INFO  {}", message); });
                auto warning_connection = session.warning.connect([&](const auto& message) { std::println("WARN  {}", message); });
                auto error_connection = session.error.connect([&](const auto& message) { std::println("ERROR {}", message); });

                message_received_connection = session.message_received.connect([&](const auto& message) {
                    std::println("IN {}", message.MsgType());
                    message.pretty_print(std::cout);
                    if (message.MsgType() == crocofix::FIX_5_0SP2::message::NewOrderSingle::MsgType) {
                        process_new_order_single(session, message, io_context);
                    }
                });

                message_sent_connection = session.message_sent.connect([&](const auto& message) {
                    std::println("OUT {}", message.MsgType());
                    message.pretty_print(std::cout);
                });

                session.open();
                scheduler.run();
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}