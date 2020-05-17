#ifndef crocofix_libcrocofix_session_options_hpp
#define crocofix_libcrocofix_session_options_hpp

#include "behaviour.hpp"

namespace crocofix
{

class session_options
{
public:

    behaviour logon_behaviour() const noexcept { return m_logon_behaviour; }
    void logon_behaviour(behaviour behaviour) noexcept { m_logon_behaviour = behaviour; }    

    const std::string& sender_comp_id() const noexcept { return m_sender_comp_id; }
    void sender_comp_id(const std::string& sender_comp_id) noexcept { m_sender_comp_id = sender_comp_id; }    

    const std::string& target_comp_id() const noexcept { return m_target_comp_id; }
    void target_comp_id(const std::string& target_comp_id) noexcept { m_target_comp_id = target_comp_id; }

    uint32_t heartbeat_interval() const noexcept { return m_heartbeat_interval; }
    void heartbeat_interval(uint32_t interval) noexcept { m_heartbeat_interval = interval; }

    uint32_t test_request_delay() const noexcept { return m_test_request_delay; }
    void test_request_delay(uint32_t delay) noexcept { m_test_request_delay = delay; }

private:

    behaviour m_logon_behaviour = behaviour::initiator;
    std::string m_sender_comp_id;
    std::string m_target_comp_id;
    uint32_t m_heartbeat_interval = 30;
    uint32_t m_test_request_delay = 1;

};

}

#endif