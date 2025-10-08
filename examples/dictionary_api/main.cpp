#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include <libcrocofixdictionary/fix50SP2_messages.hpp>
#include <iostream>
#include <print>

using namespace crocofix;

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        std::println("Reference an enumerated field value directly");
        const auto& buy = FIX_5_0SP2::field::Side::Buy;
        std::println("{} = {}", buy.name(), buy.value());

        std::println("Display a field's properties");
        auto side = FIX_5_0SP2::field::Side();
    
        std::println("{}", side.name());
        std::println("{{");
        std::println("    Tag      = {}", side.tag()); 
        std::println("    Type     = {}", side.type()); 
        std::println("    Added    = {}", side.pedigree().added()); 
        std::println("    Synopsis = {}", side.synopsis());
        std::println("}}\n"); 

        std::println("Iterate over all enumerated values for a field\n");

        // TODO - make values() static
        std::println("{}", side.name());
        std::println("{{");
        for (const auto& value : side.values())
        {
            std::println("    {} - {}", value.get().value(), value.get().name());
        }
        std::println("}}\n");

        std::println("Is 0 a valid field tag? {}", FIX_5_0SP2::fields()[0].is_valid()); 
        std::println("Is 54 a valid field tag? {}", FIX_5_0SP2::fields()[54].is_valid());
        
        std::println("What is the name of field with tag 100? {}", FIX_5_0SP2::fields().name_of_field(100));
        std::println("What is the name of field with tag 99999? {}", FIX_5_0SP2::fields().name_of_field(99999));

        std::println("What is the name of enumerated field value G for field tag 18? {}", FIX_5_0SP2::fields().name_of_value(18, "G"));
        std::println("What is the name of enumerated field value 1 for field tag 99999? {}", FIX_5_0SP2::fields().name_of_value(999999, "1"));

        std::println("tag of field with name 'OrdStatus' is {}", FIX_5_0SP2::fields()["OrdStatus"].tag());

        
        std::println("FIX_5_0SP2 has {} messages defined\n", FIX_5_0SP2::messages().size());

        std::println("Lookup a message by index in the messages() collection\n");
        
        auto heartbeat = FIX_5_0SP2::messages()[0];

        std::println("{}", heartbeat.name());
        std::println("{{"); 
        std::println("    MsgType  = {}", heartbeat.msg_type());
        std::println("    Category = {}", heartbeat.category());
        std::println("    Added    = {}", heartbeat.pedigree().added());   
        std::println("    Synopsis = {}", heartbeat.synopsis());
        std::println("}}\n");
    
        std::println("Lookup a message by MsgType in the messages() collection\n");

        auto executionReport = FIX_5_0SP2::messages()["8"];

        std::println("{}", executionReport.name());
        std::println("{{"); 
        std::println("    MsgType  = {}", executionReport.msg_type());
        std::println("    Category = {}", executionReport.category());
        std::println("    Added    = {}", executionReport.pedigree().added());        
        std::println("    Synopsis = {}", executionReport.synopsis());
        std::println("}}\n");

        std::println("{} has {} fields defined\n", heartbeat.name(), heartbeat.fields().size());

        std::println("{} Fields\n{{", heartbeat.name());

        for (const auto& field : heartbeat.fields())
        {
            std::println("     {}", field.name());
        }

        std::println("}}\n");

        std::println("Lookup the name of a message via it's MsgType A = {}\n", FIX_5_0SP2::messages().name_of_message("A"));

        std::println("FIX 5.0 SP2 Messages");
        std::println("{{");
        for (const auto& message : FIX_5_0SP2::messages())
        {
            std::println("     {} - {}", message.msg_type(), message.name());
        }
        std::println("}}\n");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
 
    return 0;
}