#ifndef crocofix_libcrocofix_tests_session_fixture_hpp
#define crocofix_libcrocofix_tests_session_fixture_hpp

#include <libcrocofix/session.hpp>
#include "test_reader.hpp"
#include "test_writer.hpp"

namespace crocofix
{

class session_fixture
{
public:

    session_fixture();

protected:

    void perform_default_logon_sequence();

    void sent_from_initiator(const std::string& msg_type, std::initializer_list<crocofix::field> fields);
    void receive_at_acceptor(const std::string& msg_type, std::initializer_list<crocofix::field> fields);
    void sent_from_acceptor(const std::string& msg_type, std::initializer_list<crocofix::field> fields);
    void receive_at_initiator(const std::string& msg_type, std::initializer_list<crocofix::field> fields);

    crocofix::test_reader initiator_reader;
    crocofix::test_writer initiator_writer;
    crocofix::session initiator;

    crocofix::test_reader acceptor_reader;
    crocofix::test_writer acceptor_writer;
    crocofix::session acceptor;

};

}

#endif