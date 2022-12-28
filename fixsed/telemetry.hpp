#ifndef crocofix_fixsed_telemetry_hpp
#define crocofix_fixsed_telemetry_hpp

namespace metrics::fixsed
{

extern const char* initiator_fix_message_in;
extern const char* initiator_fix_message_out;
extern const char* acceptor_fix_message_in;
extern const char* acceptor_fix_message_out;

}

void initialise_telemetry();

#endif