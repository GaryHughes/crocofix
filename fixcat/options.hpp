#ifndef crocofix_fixcat_options_hpp
#define crocofix_fixcat_options_hpp

#include <iostream>
#include <vector>
#include <libcrocofixdictionary/orchestration_field.hpp>

class options
{
public:

    using input_file_collection = std::vector<std::string>;
    using field_collection = std::vector<crocofix::dictionary::orchestration_field>;

    bool parse(int argc, const char** argv);
   
    bool help() const;
    bool include_admin_messages() const;
    bool mix() const;
    bool track_orders() const;
    const std::optional<field_collection>& fields() const; 
    const input_file_collection& input_files() const;

private:

    std::string m_program;
    bool m_help = false;
    bool m_include_admin_messages = false;
    bool m_mix = false;
    bool m_track_orders = false;
    std::optional<field_collection> m_fields;
    input_file_collection m_input_files;

    void parse_fields(const std::string& input);

};

#endif
