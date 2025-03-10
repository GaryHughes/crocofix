#ifndef crocofix_utilities_read_file_hpp
#define crocofix_utilities_read_file_hpp

#include <iostream>
#include <functional>
#include <string>

namespace crocofix
{

void read_file(const std::string& filename, const std::function<void(std::istream&)>& reader);

}

#endif