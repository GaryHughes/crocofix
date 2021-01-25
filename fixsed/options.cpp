#include "options.hpp"
#include <boost/program_options.hpp>
#include <libgen.h>

namespace po = boost::program_options;

static const char* option_help = "help";
static const char* option_in = "in";
static const char* option_out = "out";
static const char* option_bind = "bind";
static const char* option_pretty = "pretty";
static const char* option_script_path = "script-path";
static const char* option_script = "script";
static const char* option_log_level = "log-level";
static const char* option_log_path = "log-path";

bool options::parse(int argc, const char** argv)
{
    m_program = argv[0];

    std::string in;
    std::string out;
    std::string bind;
    std::string script_path = ".";
    std::string script_file;
    std::string log_level;
    std::string log_path = ".";

    po::options_description options;
    
    options.add_options()
        (option_help,           "display usage")
        (option_pretty,         "pretty print messages")
        (option_in,             po::value<std::string>(&in)->required(), "listen [address:]port eg. hostname:8888 or 8888")
        (option_out,            po::value<std::string>(&out)->required(), "remote address:port h eg. hostname:6666")
        (option_bind,           po::value<std::string>(&bind), "local address[:port] to bind for outgoing connection eg. hostname:6666")
        (option_script_path,    po::value<std::string>(&script_path), "the directory to load scripts from - defaults to .")
        (option_script,         po::value<std::string>(&script_file)->required(), "the filename of the lua script in --script-path to load")
        (option_log_level,      po::value<std::string>(&log_level), "the minimum log severity to include in the logger output. (trace|debug|info|warning|error|critical)")
        (option_log_path,       po::value<std::string>(&log_path), "the directory to write log files in");

    po::variables_map variables;
    po::store(po::command_line_parser(argc, argv).options(options).run(), variables);

    if (variables.count(option_help)) {
        std::cout << "usage: " << basename(const_cast<char*>(m_program.c_str())) << " [--help] [--log-level <level>] [--log-path <directory>] [--pretty] --in [address:]port --out address:port [--script-path <path>] --script <filename>\n"
                  << options << std::endl;
        m_help = true;
        return true;
    }

    m_pretty_print = variables.count(option_pretty);

    po::notify(variables);

    try
    {
        process_in(in);
        process_out(out); 
        process_bind(bind);
        process_script(script_path, script_file);
        process_log_level(log_level);
        process_log_path(log_path);
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return false;    
    }

    return true;
}

void options::process_log_path(const std::string& path)
{
    if (!std::filesystem::is_directory(path)) {
        throw std::runtime_error("--log-path " + path + " does not exist or is not a directory");
    }

    m_log_path = path;
}

void options::process_log_level(const std::string& level)
{
    if (level.empty()) {
        m_log_level = boost::log::trivial::info;
        return;
    }

    if (!boost::log::trivial::from_string(level.c_str(), level.length(), m_log_level)) {
        throw std::runtime_error("--log-level " + level + " is not valid, must be trace|debug|info|warning|error|fatal");        
    }
}

void options::process_script(const std::string& path, const std::string& file)
{
    if (!std::filesystem::is_directory(path)) {
        throw std::runtime_error("--script-path " + path + " does not exist or is not a directory");
    }

    m_script = std::filesystem::path(path) / file;

    if (!std::filesystem::is_regular_file(m_script)) {
        throw std::runtime_error("--script " + file + " does not exist or is not a regular file in --script-path " + path);
    }
}

void options::process_in(const std::string& in)
{
    auto colon = in.find(':');

    if (colon == std::string::npos) {
        m_in_port = std::stoi(in);
    }
    else {
        m_in_host = in.substr(0, colon);
        m_in_port = std::stoi(in.substr(colon + 1));
    }
}

void options::process_out(const std::string& out)
{
    auto colon = out.find(':');

    if (colon == std::string::npos) {
        throw std::runtime_error("malformed value for --out, it must contain a host and port separated by a :");
    }

    m_out_host = out.substr(0, colon);
    m_out_port = std::stoi(out.substr(colon + 1));
}

void options::process_bind(const std::string& bind)
{
    if (bind.empty()) {
        return;
    }
    
    auto colon = bind.find(':');

    if (colon == std::string::npos) {
        m_bind_port = std::stoi(bind);
    }
    else {
        m_bind_host = bind.substr(0, colon);
        m_bind_port = std::stoi(bind.substr(colon + 1));
    }
}
