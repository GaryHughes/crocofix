#ifndef crocofix_fixsed_logging_hpp
#define crocofix_fixsed_logging_hpp

#include <filesystem>
#include <spdlog/spdlog.h>

void initialise_logging(const std::string& program, 
                        spdlog::level::level_enum level,
                        const std::filesystem::path& directory);

#endif