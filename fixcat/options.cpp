#include "options.hpp"
#include <exception>
#include <boost/program_options.hpp>
#include <libgen.h>
#include <sys/param.h>

namespace po = boost::program_options;

static char const * const option_help = "help";
static char const * const option_admin = "admin";
static char const * const option_files = "files";
static char const * const option_orders = "orders";

bool options::parse(int argc, const char** argv)
{
    m_program = *argv;

    po::options_description options;
    
    options.add_options()
        (option_help, "display usage")
        (option_admin, "include administrative messages")
        (option_orders, "track order state")
        (option_files, po::value<input_file_collection>());

    po::positional_options_description positional;
    positional.add(option_files, -1);

    po::variables_map variables;
    po::store(po::command_line_parser(argc, argv).options(options).positional(positional).run(), variables);

    if (variables.count(option_help) > 0)
    {
        const char* name = nullptr;
#if __linux__
        name = basename(const_cast<char*>(m_program.c_str())); // NOLINT(cppcoreguidelines-pro-type-const-cast)
#else
        std::array<char, MAXPATHLEN> buffer{};

        if (basename_r(m_program.c_str(), buffer.begin()) == nullptr)
        {
            std::ostringstream msg;
            msg << "basename_r(\"" << m_program << "\") failed";
            throw std::runtime_error(msg.str());
        }

        name = buffer.begin();
#endif

        std::cout << "usage: " << name << " [--help] [--admin] [--orders] [FILE]...\n"
                  << options << std::endl;

        m_help = true;
        return true;
    }

    po::notify(variables);

    m_include_admin_messages = variables.count(option_admin) > 0;
    m_track_orders = variables.count(option_orders) > 0;
  
    if (variables.count(option_files) > 0)
    {
        m_input_files = variables[option_files].as<input_file_collection>();
    }

    return true;
}

bool options::help() const
{
    return m_help;
}

bool options::include_admin_messages() const
{
    return m_include_admin_messages;
}

bool options::track_orders() const
{
    return m_track_orders;
}

const options::input_file_collection& options::input_files() const
{
    return m_input_files;
}

