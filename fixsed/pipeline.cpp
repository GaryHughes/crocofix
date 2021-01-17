#include "pipeline.hpp"
#include <iostream>
#include <sstream>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include <libcrocofix/socket_reader.hpp>
#include <libcrocofix/socket_writer.hpp>
#include <sol/sol.hpp>

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

        crocofix::socket_reader initiator_reader(initiator_socket);
        crocofix::socket_writer initiator_writer(initiator_socket);

        crocofix::socket_reader acceptor_reader(acceptor_socket);
        crocofix::socket_writer acceptor_writer(acceptor_socket);

        sol::state lua;

        initiator_reader.read_async([&](crocofix::message& message)
        {
            log_message(logger, message);
            acceptor_writer.write(message);       
        });

        acceptor_reader.read_async([&](crocofix::message& message)
        {
            log_message(logger, message);
            initiator_writer.write(message);
        });

        io_context.run();
    }
    catch(std::exception& ex)
    {
        log_error(logger) << "error: " << ex.what();
    }

}