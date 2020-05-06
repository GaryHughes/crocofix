#ifndef crocofix_fixsed_logging_hpp
#define crocofix_fixsed_logging_hpp

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>

#define log_trace(logger)   BOOST_LOG_SEV(logger, boost::log::trivial::severity_level::trace)
#define log_debug(logger)   BOOST_LOG_SEV(logger, boost::log::trivial::severity_level::debug)
#define log_info(logger)    BOOST_LOG_SEV(logger, boost::log::trivial::severity_level::info)
#define log_warning(logger) BOOST_LOG_SEV(logger, boost::log::trivial::severity_level::warning)
#define log_error(logger)   BOOST_LOG_SEV(logger, boost::log::trivial::severity_level::error)
#define log_fatal(logger)   BOOST_LOG_SEV(logger, boost::log::trivial::severity_level::fatal)

void initialise_logging(const std::string& program);

#endif