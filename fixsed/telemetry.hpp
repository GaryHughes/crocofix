#ifndef crocofix_fixsed_telemetry_hpp
#define crocofix_fixsed_telemetry_hpp

namespace metrics::fixsed
{

// These are counters for the number of messages in each direction.
extern const char* initiator_fix_message_in;
extern const char* initiator_fix_message_out;
extern const char* acceptor_fix_message_in;
extern const char* acceptor_fix_message_out;

// These are histograms for the processing time in microseconds for messages in each direction.
extern const char* initiator_fix_message_process;
extern const char* acceptor_fix_message_process;

}

void initialise_telemetry();

#endif