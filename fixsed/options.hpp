#ifndef crocofix_fixsed_options_hpp
#define crocofix_fixsed_options_hpp

#include <iostream>
#include <vector>
#include <optional>
#include <filesystem>
#include <spdlog/spdlog.h>

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
    spdlog::level::level_enum log_level() const { return m_log_level; }
    const std::filesystem::path& log_path() const { return m_log_path; }

private:

    void process_in(const std::string& in);
    void process_out(const std::string& out);
    void process_bind(const std::string& bind);
    void process_script(const std::string& path, const std::string& file);
    void process_log_level(const std::string& level);
    void process_log_path(const std::string& path);

    std::string m_program;

    bool m_help = false;
    bool m_pretty_print = false;

    std::optional<std::string> m_in_host;
    int m_in_port;

	std::string	m_out_host;
    int m_out_port;

	std::optional<std::string> m_bind_host;
	std::optional<int> m_bind_port;

    std::filesystem::path m_script;

    spdlog::level::level_enum m_log_level;

    std::filesystem::path m_log_path;

};

#endif
