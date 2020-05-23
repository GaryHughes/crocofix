#ifndef crocofix_libcrocofix_session_hpp
#define crocofix_libcrocofix_session_hpp

#include "reader.hpp"
#include "writer.hpp"
#include "session_options.hpp"
#include "session_state.hpp"
#include "scheduler.hpp"
#include <boost/signals2.hpp>

namespace crocofix
{

class session
{
public:

    boost::signals2::signal<void (const std::string& message)> error;

    boost::signals2::signal<void (session_state from, session_state to)> state_changed;


    session(reader& reader, 
            writer& writer,
            scheduler& scheduler);

    void open();
    void close();

    session_state state() const;

    void send(message& message, int options = encode_options::standard);

    behaviour logon_behaviour() const noexcept;
    void logon_behaviour(behaviour behaviour) noexcept;

    const std::string& begin_string() const noexcept;
    void begin_string(const std::string& begin_string);    

    const std::string& sender_comp_id() const noexcept;
    void sender_comp_id(const std::string& sender_comp_id);    

    const std::string& target_comp_id() const noexcept;
    void target_comp_id(const std::string& target_comp_id) noexcept;
   
    uint32_t heartbeat_interval() const noexcept;
    void heartbeat_interval(uint32_t interval) noexcept;

    uint32_t test_request_delay() const noexcept;
    void test_request_delay(uint32_t delay) noexcept;

private:

    void state(session_state state);

    void ensure_options_are_mutable();
    void on_message_read(crocofix::message& message);

    void logon();
    
    bool process_logon(const crocofix::message& logon);
    void process_test_request(const crocofix::message& test_request);
    void process_heartbeat(const crocofix::message& heartbreat);
    
    void send_logon(bool reset_seq_num_flag = false);
    void send_logout(const std::string& text);
    void send_post_logon_test_tequest();
    void send_test_request();
    void send_reject(message message, const std::string& text);

    bool extract_heartbeat_interval(const crocofix::message& logon);
    bool validate_checksum(const crocofix::message& message);
    bool validate_body_length(const crocofix::message& message);
    bool validate_comp_ids(const crocofix::message& message);
    bool validate_first_message(const crocofix::message& message);
    
    uint32_t allocate_test_request_id();
    uint32_t allocate_outgoing_msg_seq_num();

    void start_defibrillator();
    void stop_defibrillator();

    reader& m_reader;
    writer& m_writer;
    scheduler& m_scheduler;

    session_options m_options;
    session_state m_state = session_state::connected;
    bool m_logon_received = false;

    std::optional<uint32_t> m_expected_test_request_id;
    uint32_t m_next_test_request_id = 0;
    uint32_t m_next_outgoing_msg_seq_num = 1;

};

}

#endif