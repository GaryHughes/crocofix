#include "logging.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <unistd.h>

void initialise_logging(const std::string& program, spdlog::level::level_enum level, const std::filesystem::path& directory)
{
    const std::string filename = directory / std::format("{}_{}_%Y%m%d_%H%M%S_%3N.log", program, getpid());

    auto max_size = 1024 * 1024 * 100;
    auto max_files = 3;
    auto logger = spdlog::rotating_logger_mt("fixsed", filename, max_size, max_files);

    spdlog::set_level(level);

    // boost::log::add_file_log
    // (
    //     boost::log::keywords::file_name = directory / filename,
    //     boost::log::keywords::rotation_size = 100 * 1024 * 1024,
    //     boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
    //     boost::log::keywords::format = 
    //     (
    //         boost::log::expressions::stream
    //             << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f (") 
    //             << boost::log::trivial::severity
    //             << ") " << boost::log::expressions::smessage
    //     ),
    //     boost::log::keywords::auto_flush = true
    // );

    // boost::log::core::get()->set_filter
    // (
    //     boost::log::trivial::severity >= level
    // );

    // boost::log::add_common_attributes();
}


