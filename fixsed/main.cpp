#include <iostream>
#include "logging.hpp"
#include "pipeline.hpp"

int main(int argc, const char** argv)
{
    try
    {
        options options;

        if (!options.parse(argc, argv) || options.help()) {
            return 1;
        }

        initialise_logging(options.program());

        while (true)
        {
            pipeline(options).run();
        }
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}