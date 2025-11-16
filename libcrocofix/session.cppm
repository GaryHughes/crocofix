module;

#include <libcrocofixdictionary/orchestration.hpp>
#include <boost/signals2.hpp>

export module crocofix:session;

import :message;
import :session_state;
import :session_options;
import :scheduler;
import :reader;
import :writer;
import :behaviour;

export namespace crocofix
{

class session
{
public:

    using log_signal = boost::signals2::signal<void (const std::string& message)>;
    using state_changed_signal = boost::signals2::signal<void (session_state from, session_state to)>;
    using message_signal = boost::signals2::signal<void (const message& message)>;

    log_signal error;
    log_signal warning;
    log_signal information;
    
    state_changed_signal state_changed;

    message_signal message_sent;
    message_signal message_received;

    session(reader& reader, 
            writer& writer,
            scheduler& scheduler,
            const dictionary::orchestration& orchestration);

    void open();
    void close();

    session_state state() const;

    void send(message& message, int options = encode_options::standard);

    behaviour logon_behaviour() const noexcept;
    void logon_behaviour(behaviour behaviour);

    const std::string& begin_string() const noexcept;
    void begin_string(const std::string& begin_string);    

    const std::string& sender_comp_id() const noexcept;
    void sender_comp_id(const std::string& sender_comp_id);    

    const std::string& target_comp_id() const noexcept;
    void target_comp_id(const std::string& target_comp_id);
   
    uint32_t heartbeat_interval() const noexcept;
    void heartbeat_interval(uint32_t interval);

    uint32_t test_request_delay() const noexcept;
    void test_request_delay(uint32_t delay);

    crocofix::timestamp_format timestamp_format() const noexcept;
    void timestamp_format(crocofix::timestamp_format format) noexcept;

    uint32_t incoming_msg_seq_num() const noexcept;
    void incoming_msg_seq_num(uint32_t msg_seq_num) noexcept;

    uint32_t outgoing_msg_seq_num() const noexcept;
    void outgoing_msg_seq_num(uint32_t msg_seq_num) noexcept;

    bool use_next_expected_msg_seq_num() const noexcept;
    void use_next_expected_msg_seq_num(bool value) noexcept;

private:

    void state(session_state state);

    void ensure_options_are_mutable() const;
    void on_message_read(crocofix::message& message);

    void logon();
    
    bool process_logon(const crocofix::message& logon);
    void process_logout(const crocofix::message& logout);
    void process_test_request(const crocofix::message& test_request);
    void process_heartbeat(const crocofix::message& heartbeat);
    void process_sequence_reset(const crocofix::message& sequence_reset, bool poss_dup);
    void process_resend_request(const crocofix::message& resend_request);
    
    void send_logon(bool reset_seq_num_flag = false);
    void send_logout(const std::string& text);
    void send_post_logon_test_request();
    void send_test_request();
    void send_reject(const message& message, const std::string& text, std::optional<dictionary::field_value> reason = std::nullopt);
    void send_gap_fill(uint32_t msg_seq_num, uint32_t new_seq_no);

    void perform_resend(uint32_t begin_msg_seq_num, uint32_t end_msg_seq_num);

    bool extract_heartbeat_interval(const crocofix::message& logon);
    bool validate_checksum(const crocofix::message& message);
    bool validate_body_length(const crocofix::message& message);
    bool validate_begin_string(const crocofix::message& message);
    bool validate_comp_ids(const crocofix::message& message);
    bool validate_first_message(const crocofix::message& message);
    bool validate_sequence_numbers(const crocofix::message& message);

    bool sequence_number_is_high(const crocofix::message& message);
    bool sequence_number_is_low(const crocofix::message& message);
    
    void request_resend(uint32_t received_msg_seq_num);
    void reset();

    uint32_t allocate_test_request_id();
    uint32_t allocate_outgoing_msg_seq_num();

    void start_defibrillator();
    void defibrillate();
    void stop_defibrillator();
    void stop_test_request_timer();

    reader& m_reader;
    writer& m_writer;
    scheduler& m_scheduler;
    const dictionary::orchestration& m_orchestration;

    session_options m_options;
    session_state m_state = session_state::connected;
    bool m_logon_received = false;

    std::optional<uint32_t> m_expected_test_request_id;
    uint32_t m_next_test_request_id = 0;
    
    // TODO - these two need to go into a struct for persistence
    uint32_t m_incoming_msg_seq_num = 1;
    uint32_t m_outgoing_msg_seq_num = 1;

    uint32_t m_incoming_resent_msg_seq_num = 0;
    uint32_t m_incoming_target_msg_seq_num = 0;

    std::optional<scheduler::cancellation_token> m_test_request_timer_token;
    std::optional<scheduler::cancellation_token> m_heartbeat_timer_token;

    boost::signals2::connection m_reader_message_read;
    boost::signals2::connection m_reader_closed;
    boost::signals2::connection m_writer_closed;

};

}
