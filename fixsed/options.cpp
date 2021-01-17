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

bool options::parse(int argc, const char** argv)
{
    m_program = argv[0];

    po::options_description options;
    
    options.add_options()
        (option_help,           "display usage")
        (option_pretty,         "pretty print messages")
        (option_in,             po::value<std::string>(&m_in)->required(), "listen [address:]port eg. hostname:8888 or 8888")
        (option_out,            po::value<std::string>(&m_out)->required(), "remote address:port h eg. hostname:6666")
        (option_bind,           po::value<std::string>(&m_bind), "local address[:port] to bind for outgoing connection eg. hostname:6666")
        (option_script_path,    po::value<std::string>(&m_script_path), "the directory to load scripts from - defaults to .")
        (option_script,         po::value<std::string>(&m_script_file)->required(), "the filename of the lua script in --script-path to load");

    po::variables_map variables;
    po::store(po::command_line_parser(argc, argv).options(options).run(), variables);

    if (variables.count(option_help)) {
        std::cout << "usage: " << basename(const_cast<char*>(m_program.c_str())) << " [--help] [--pretty] --in [address:]port --out address:port [--script-path <path>] --script <filename>\n";
        m_help = true;
        return true;
    }

    m_pretty_print = variables.count(option_pretty);

    po::notify(variables);

    try
    {
        process_in();
        process_out(); 
        process_bind();
        process_script();
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return false;    
    }

    return true;
}

void options::process_script()
{
    if (!std::filesystem::is_directory(m_script_path)) {
        throw std::runtime_error("--script-path " + m_script_path + " does not exist or is not a directory");
    }

    m_script = std::filesystem::path(m_script_path) / m_script_file;

    if (!std::filesystem::is_regular_file(m_script)) {
        throw std::runtime_error("--script " + m_script_file + " does not exist or is not a regular file in --script-path " + m_script_path);
    }
}

void options::process_in()
{
    auto colon = m_in.find(':');

    if (colon == std::string::npos) {
        m_in_port = std::stoi(m_in);
    }
    else {
        m_in_host = m_in.substr(0, colon);
        m_in_port = std::stoi(m_in.substr(colon + 1));
    }
}

void options::process_out()
{
    auto colon = m_out.find(':');

    if (colon == std::string::npos) {
        throw std::runtime_error("malformed value for --out, it must contain a host and port separated by a :");
    }

    m_out_host = m_out.substr(0, colon);
    m_out_port = std::stoi(m_out.substr(colon + 1));
}

void options::process_bind()
{
    if (m_bind.empty()) {
        return;
    }
    
    auto colon = m_bind.find(':');

    if (colon == std::string::npos) {
        m_bind_port = std::stoi(m_bind);
    }
    else {
        m_bind_host = m_bind.substr(0, colon);
        m_bind_port = std::stoi(m_bind.substr(colon + 1));
    }
}
