#ifndef crocofix_libcrocofix_tests_session_fixture_hpp
#define crocofix_libcrocofix_tests_session_fixture_hpp

#include <libcrocofix/session.hpp>
#include <libcrocofix/boost_asio_scheduler.hpp>
#include <libcrocofixutility/blocking_queue.hpp>
#include <libcrocofixdictionary/orchestration.hpp>
#include "test_reader.hpp"
#include "test_writer.hpp"
#include <queue>
#include <chrono>
#include <boost/thread.hpp>
namespace crocofix
{

const std::chrono::milliseconds default_timeout = std::chrono::milliseconds(5000);

class session_fixture
{
public:

    session_fixture();
    ~session_fixture();
   
protected:

    void perform_default_logon_sequence();

    bool sent_from_initiator(const std::string& msg_type, 
                             const std::initializer_list<crocofix::field> fields = {},
                             const std::chrono::milliseconds timeout = default_timeout);

    bool received_at_acceptor(const std::string& msg_type, 
                              const std::initializer_list<crocofix::field> fields = {},
                              const std::chrono::milliseconds timeout = default_timeout);
    
    bool sent_from_acceptor(const std::string& msg_type, 
                            const std::initializer_list<crocofix::field> fields = {},
                            const std::chrono::milliseconds timeout = default_timeout);
    
    bool received_at_initiator(const std::string& msg_type, 
                               const std::initializer_list<crocofix::field> fields = {},
                               const std::chrono::milliseconds timeout = default_timeout);

    bool received_at_initiator(const std::string& msg_type, 
                               const std::function<void(const message&)>& validator,
                               const std::initializer_list<crocofix::field> fields = {},
                               const std::chrono::milliseconds timeout = default_timeout);

    static bool expect(blocking_queue<message>& messages,
                       const std::string& msg_type,
                       const std::optional<std::function<void(const message&)>>& validator,
                       const std::initializer_list<crocofix::field> fields,
                       const std::chrono::milliseconds timeout);

    bool initiator_state_change(session_state expected_state,
                                const std::chrono::milliseconds timeout = default_timeout);

    bool acceptor_state_change(session_state expected_state,
                               const std::chrono::milliseconds timeout = default_timeout);

    static bool expect_state_change(blocking_queue<crocofix::session_state>& state_changes,
                                    session_state expected_state,
                                    const std::chrono::milliseconds timeout = default_timeout);

    static void send(crocofix::session& session,
                     const std::string& msg_type,
                     std::initializer_list<field> fields = {},
                     int options = encode_options::standard,
                     std::initializer_list<int> fields_to_remove = {});

    void send_from_initiator(const std::string& msg_type,
                             std::initializer_list<field> fields = {},
                             int options = encode_options::standard,
                             std::initializer_list<int> fields_to_remove = {});

    void send_from_acceptor(const std::string& msg_type,
                            std::initializer_list<field> fields = {},
                            int options = encode_options::standard,
                            std::initializer_list<int> fields_to_remove = {});                           

    blocking_queue<crocofix::message> initiator_outgoing_messages;
    blocking_queue<crocofix::message> initiator_incoming_messages;

    blocking_queue<crocofix::message> acceptor_outgoing_messages;
    blocking_queue<crocofix::message> acceptor_incoming_messages;

    blocking_queue<crocofix::session_state> initiator_state_changes;
    blocking_queue<crocofix::session_state> acceptor_state_changes;

    crocofix::dictionary::orchestration orchestration;

    boost::asio::io_context io_context;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard;
    crocofix::boost_asio_scheduler scheduler;

    crocofix::test_reader initiator_reader;
    crocofix::test_writer initiator_writer;
    crocofix::session initiator;

    crocofix::test_reader acceptor_reader;
    crocofix::test_writer acceptor_writer;
    crocofix::session acceptor;

    boost::thread scheduler_thread;

    boost::signals2::connection initiator_state_change_connection;
    boost::signals2::connection acceptor_state_change_connection;    
};

}

#endif