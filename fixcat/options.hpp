#ifndef crocofix_fixcat_options_hpp
#define crocofix_fixcat_options_hpp

#include <iostream>
#include <vector>

class options
{
public:

    using input_file_collection = std::vector<std::string>;

    bool parse(int argc, const char** argv);
   
    bool help() const;
    bool include_admin_messages() const;
    bool track_orders() const;
    const input_file_collection& input_files() const;

private:

    std::string m_program;
    bool m_help = false;
    bool m_include_admin_messages = false;
    bool m_track_orders = false;
    input_file_collection m_input_files;

};

#endif
