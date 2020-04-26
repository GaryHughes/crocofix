#include <boost/algorithm/string.hpp>
#include "options.hpp"
#include "libutility/read_file.hpp"

using namespace crocofix;

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
            read_file(filename, [&options](std::istream& is)
            {
                for (;;)
                {
                    std::string line;
                    
                    if (!std::getline(is, line))
                    {
                        break;
                    }

                    
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