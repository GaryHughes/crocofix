#include <iostream>
#include <boost/algorithm/string.hpp>
#include "options.hpp"
#include "libcrocofixutility/read_file.hpp"
#include "libcrocofix/message.hpp"

static constexpr const char* fix_message_prefix = "8=FIX";

void decode_and_print_line(const options& options, const std::string& line)
{
    try
    {
        auto start_of_message = line.find(fix_message_prefix);

        if (start_of_message == std::string::npos) {
            return;
        }

        crocofix::message message;
        
        message.decode(line.substr(start_of_message));

        if (!options.include_admin_messages() && message.is_admin()) {
            return;
        }

        message.pretty_print(std::cout);
        std::cout << '\n';
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
}

void process_stream(const options& options, std::istream& stream)
{
    for (;;)
    {
        std::string line;
        
        if (!std::getline(stream, line)) {
            break;
        }

        decode_and_print_line(options, line);    
    }
}

int main(int argc, const char** argv)
{
    try
    {
        options options;

        if (!options.parse(argc, argv) || options.help()) {
            return 1;
        }

        if (options.input_files().empty())
        {
            process_stream(options, std::cin);
        }
        else
        {
            for (const auto& filename : options.input_files())
            {
                crocofix::read_file(filename, [&options](std::istream& stream)
                {
                    process_stream(options, stream);
                });
            }
        }
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}