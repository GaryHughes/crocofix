#include <boost/algorithm/string.hpp>
#include "options.hpp"
#include "libutility/read_file.hpp"
#include "libcrocofix/message.hpp"

static constexpr const char* fix_message_prefix = "8=FIX";

void decode_and_print_line(const std::string& line)
{
    try
    {
        auto start_of_message = line.find(fix_message_prefix);

        if (start_of_message == std::string::npos) {
            return;
        }

        crocofix::message message;
        message.decode(line.substr(start_of_message));
        std::cout << "MESSAGE " << message.fields().size() << std::endl;
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
}

int main(int argc, const char** argv)
{
    try
    {
        options options;

        if (!options.parse(argc, argv) || options.help())
        {
            options.usage(std::cerr);
            return 1;
        }

        for (const auto filename : options.input_files())
        {
            crocofix::read_file(filename, [&options](std::istream& is)
            {
                for (;;)
                {
                    std::string line;
                    
                    if (!std::getline(is, line))
                    {
                        break;
                    }

                    decode_and_print_line(line);    
                }
            });
        }
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}