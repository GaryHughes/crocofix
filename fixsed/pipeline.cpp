import crocofix;
import crocofix.dictionary;
import crocofix.lua;

#include "pipeline.hpp"
#include "probes.h"
#include <iostream>
#include <sstream>
#include <sol/sol.hpp>

using boost::asio::ip::tcp;

pipeline::pipeline(const options& options)
:   m_options(options)
{
}

void pipeline::log_message(const crocofix::message& message)
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

    spdlog::info(msg.str());
}

void pipeline::run() // NOLINT(readability-function-cognitive-complexity)
{
    try
    {
        boost::asio::io_context io_context;
        
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), m_options.in_port()));

        // TODO - bind host

        acceptor.listen();
        tcp::socket initiator_socket(io_context); // NOLINT(clang-analyzer-optin.cplusplus.UninitializedObject)

        std::string in_host = "*";
        auto optional = m_options.in_host();
        if (optional.has_value()) {
            in_host = optional.value();
        }

	    spdlog::info("waiting for initiator [{}:{}]", in_host, m_options.in_port());

        acceptor.accept(initiator_socket);

        spdlog::info("accepted initiator [{}:{}]", 
            initiator_socket.remote_endpoint().address().to_string(),
            initiator_socket.remote_endpoint().port());

        spdlog::info("resolving acceptor [{}:{}]", m_options.out_host(), m_options.out_port());

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(m_options.out_host(), std::to_string(m_options.out_port()));
        
        spdlog::info("connecting to acceptor [{}:{}]", m_options.out_host(), m_options.out_port());
  
        tcp::socket acceptor_socket(io_context);

        // TODO - bind

        boost::asio::connect(acceptor_socket, endpoints);

	    spdlog::info("connected to acceptor [{}:{}]", 
            acceptor_socket.remote_endpoint().address().to_string(),
            acceptor_socket.remote_endpoint().port());

        crocofix::socket_reader initiator_reader(initiator_socket);
        crocofix::socket_writer initiator_writer(initiator_socket);

        crocofix::socket_reader acceptor_reader(acceptor_socket);
        crocofix::socket_writer acceptor_writer(acceptor_socket);

        sol::state lua;
        lua.open_libraries(sol::lib::base, sol::lib::package);

        crocofix::define_lua_types(lua);

        // Do the load and validation here instead of in the options processing so we can make edits without restarting.
        lua.safe_script_file(m_options.script().c_str());
     
        auto initiator_read = lua["initiator_read"];

        if (!initiator_read.valid()) {
            throw std::runtime_error("could not load initiator_read() function from script " + m_options.script().generic_string());
        }

        auto acceptor_read = lua["acceptor_read"];

        if (!acceptor_read.valid()) {
            throw std::runtime_error("could not load acceptor_read() function from script " + m_options.script().generic_string());
        }

        lua.set_function("log_trace", [&](const std::string& message) { spdlog::trace(message); });
        lua.set_function("log_debug", [&](const std::string& message) { spdlog::debug(message); });
        lua.set_function("log_info", [&](const std::string& message) { spdlog::info(message); });
        lua.set_function("log_warn", [&](const std::string& message) { spdlog::warn(message); });
        lua.set_function("log_error", [&](const std::string& message) { spdlog::error(message); });
        lua.set_function("log_fatal", [&](const std::string& message) { spdlog::critical(message); });
       
        // TODO - add logging options, before edit, after edit, both?

        auto process_message = [&](auto& message, auto& function, auto& destination) // NOLINT(readability-function-cognitive-complexity)
        {
            log_message(message);
     
            // NOLINTBEGIN(misc-const-correctness, readability-identifier-length, cppcoreguidelines-avoid-do-while)
            if (FIXSED_SCRIPT_CALL_ENABLED()) {
                FIXSED_SCRIPT_CALL(message.SenderCompID().c_str(), 
                                   message.TargetCompID().c_str(),
                                   message.MsgType().c_str());
            }  
            // NOLINTEND(misc-const-correctness, readability-identifier-length, cppcoreguidelines-avoid-do-while)

            auto result = function(message);

            // NOLINTBEGIN(misc-const-correctness, readability-identifier-length, cppcoreguidelines-avoid-do-while)
            if (FIXSED_SCRIPT_RETURN_ENABLED()) {
                FIXSED_SCRIPT_RETURN(message.SenderCompID().c_str(),
                                     message.TargetCompID().c_str(),
                                     message.MsgType().c_str());
            }  
            // NOLINTEND(misc-const-correctness, readability-identifier-length, cppcoreguidelines-avoid-do-while)

            if (!result.valid()) {
                const sol::error err = result;
                spdlog::error(err.what());
                return;
            }

            log_message(message);

            destination.write(message);       
        };

        initiator_reader.message_read.connect([&](auto& message) {
            process_message(message, initiator_read, acceptor_writer);
        });

        acceptor_reader.message_read.connect([&](auto& message) {
            process_message(message, acceptor_read, initiator_writer);
        });

        acceptor_reader.open();
        initiator_reader.open();
       
        io_context.run();
    }
    catch(std::exception& ex)
    {
        spdlog::error("error: {}", ex.what());
    }

}