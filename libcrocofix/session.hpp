#ifndef crocofix_libcrocofix_session_hpp
#define crocofix_libcrocofix_session_hpp

#include "reader.hpp"
#include "writer.hpp"
#include "session_options.hpp"
#include "session_state.hpp"

namespace crocofix
{

class session
{
public:

    session(reader& reader, 
            writer& writer);

    void open();
    void close();

    session_state state() const;

    behaviour logon_behaviour() const noexcept;
    void logon_behaviour(behaviour behaviour) noexcept;

    const std::string& sender_comp_id() const noexcept;
    void sender_comp_id(const std::string& sender_comp_id);    

    const std::string& target_comp_id() const noexcept;
    void target_comp_id(const std::string& target_comp_id) noexcept;
   
private:

    void state(session_state state);

    void ensure_options_are_mutable();
    void on_message_read(crocofix::message& message);

    reader& m_reader;
    writer& m_writer;

    session_options m_options;
    session_state m_state = session_state::connected;
    

};

}

#endif