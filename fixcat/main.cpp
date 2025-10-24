#include <iostream>
#include <print>
#include <boost/algorithm/string.hpp>
#include "options.hpp"
#include "libcrocofixutility/read_file.hpp"
#include "libcrocofix/message.hpp"
#include "libcrocofix/order_report.hpp"

static constexpr const char* fix_message_prefix = "8=FIX";

namespace {

void decode_and_print_line(const options& options, const std::string& line, crocofix::order_book& book, crocofix::order_report& report)
{
    try
    {
        auto start_of_message = line.find(fix_message_prefix);

        if (start_of_message == std::string::npos) {
            if (options.mix()) {
                std::println("{}", line);
            }
            return;
        }

        crocofix::message message;
        
        message.decode(line.substr(start_of_message));

        if (!options.include_admin_messages() && message.is_admin()) {
            return;
        }

        message.pretty_print(std::cout);
        std::println("");

        if (options.track_orders()) {
            if (book.process(message)) {
                report.print(std::cout, book);
                std::println("");
            }
        }

    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl; // NOLINT(performance-avoid-endl)
    }
}

void process_stream(const options& options, std::istream& stream)
{
    const auto& fields = options.fields();
    crocofix::order_report report { fields.has_value() ? fields.value() : crocofix::order_report::default_fields }; 
    crocofix::order_book book;

    for (;;)
    {
        std::string line;
        
        if (!std::getline(stream, line)) {
            break;
        }

        decode_and_print_line(options, line, book, report);    
    }
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
        std::cerr << ex.what() << std::endl; // NOLINT(performance-avoid-endl)
        return 1;
    }

    return 0;
}