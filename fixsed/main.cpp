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

        initialise_logging(options.program(), 
                           options.log_level(),
                           options.log_path());

        while (true)
        {
            pipeline(options).run();
        }
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl; // NOLINT(performance-avoid-endl)
        return 1;
    }

    return 0;
}