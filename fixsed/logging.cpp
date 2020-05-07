#include "logging.hpp"
#include <boost/log/common.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/date_time.hpp>

void initialise_logging(const std::string& program)
{
    boost::log::add_file_log
    (
        boost::log::keywords::file_name = program + "_" + std::to_string(getpid()) + "_%Y%m%d_%H%M%S_%3N.log",
        boost::log::keywords::rotation_size = 100 * 1024 * 1024,
        boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        boost::log::keywords::format = 
        (
            boost::log::expressions::stream
                << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f (") 
                << boost::log::trivial::severity
                << ") " << boost::log::expressions::smessage
        ),
        boost::log::keywords::auto_flush = true
    );

    boost::log::core::get()->set_filter
    (
        boost::log::trivial::severity >= boost::log::trivial::info
    );

    boost::log::add_common_attributes();
}


