#include "options.hpp"
#include <exception>
#include <charconv>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <libgen.h>
#include <sys/param.h>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

namespace po = boost::program_options;

static char const * const option_help = "help";
static char const * const option_admin = "admin";
static char const * const option_mix = "mix";
static char const * const option_files = "files";
static char const * const option_fields = "fields";
static char const * const option_orders = "orders";

bool options::parse(int argc, const char** argv)
{
    m_program = *argv;

    po::options_description options;
    
    options.add_options()
        (option_help, "display usage")
        (option_admin, "include administrative messages")
        (option_mix, "print non FIX text in the output")
        (option_orders, "track order state")
        (option_fields, po::value<std::string>(), "comma separated list of field names or tags to display when tracking order state")
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

        std::cout << "usage: " << name << " [--help] [--admin] [--mix] [--orders] [--fields \"ClOrdID,OrderQty,CumQty,AvgPx,100,39\"] [FILE]...\n"
                  << options << std::endl;

        m_help = true;
        return true;
    }

    po::notify(variables);

    m_include_admin_messages = variables.count(option_admin) > 0;
    m_mix = variables.count(option_mix) > 0;
    m_track_orders = variables.count(option_orders) > 0;
  
    if (variables.count(option_fields) > 0) {
        auto fields = variables[option_fields].as<std::string>();
        parse_fields(fields);
    }
    else {
        auto* fields_env = std::getenv("CROCOFIX_FIXCAT_FIELDS"); // NOLINT(concurrency-mt-unsafe)
        if (fields_env != nullptr) {
            parse_fields(fields_env);
        }
    }

    if (variables.count(option_files) > 0) {
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

bool options::mix() const
{
    return m_mix;
}

bool options::track_orders() const
{
    return m_track_orders;
}

const std::optional<options::field_collection>& options::fields() const
{
    return m_fields;
} 

const options::input_file_collection& options::input_files() const
{
    return m_input_files;
}

void options::parse_fields(const std::string& input)
{
    field_collection fields;
    std::vector<std::string> tokens;
    boost::split(tokens, input, boost::is_any_of(","));
    for (auto& token : tokens) {
        boost::trim(token);
        try {
            auto field = crocofix::FIX_5_0SP2::fields()[token];
            fields.push_back(field);
        }
        catch (std::out_of_range&) {
            try {
                auto tag = std::stoi(token);
                auto field = crocofix::FIX_5_0SP2::fields()[tag];
                fields.push_back(field);
            }
            catch (const std::exception&) {
                throw std::out_of_range("Unable to find a FIX field with name or tag = '" + token + "'");
            }
        }
    }
    m_fields = fields;
}
