#ifndef crocofix_fixsed_options_hpp
#define crocofix_fixsed_options_hpp

#include <iostream>
#include <vector>
#include <optional>
#include <filesystem>

class options
{
public:

    bool parse(int argc, const char** argv);

    bool help() const { return m_help; }
    bool pretty_print() const { return m_pretty_print; }
    const std::optional<std::string>& in_host() const { return m_in_host; }
	int in_port() const { return m_in_port; }
	const std::string& out_host() const { return m_out_host; }
	int out_port() const { return m_out_port; }
	const std::optional<std::string>& bind_host() const { return m_bind_host; }
	std::optional<int> bind_port() const { return m_bind_port; }
    const std::string& program() const { return m_program; }
    const std::filesystem::path& script() const { return m_script; }

    // TODO - log path

private:

    void process_in();
    void process_out();
    void process_bind();
    void process_script();

    std::string m_program;

    // Raw options from the command line.
    std::string m_in;
    std::string m_out;
    std::string m_bind;
    std::string m_script_path = ".";
    std::string m_script_file;

    // Post processed options to make life easier for the caller.
    bool m_help = false;
    bool m_pretty_print = false;

    std::optional<std::string> m_in_host;
    int m_in_port;

	std::string	m_out_host;
    int m_out_port;

	std::optional<std::string> m_bind_host;
	std::optional<int> m_bind_port;

    std::filesystem::path m_script;  

};

#endif
