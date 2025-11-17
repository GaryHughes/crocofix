module;

#include <iostream>
#include <functional>
#include <string>

export module crocofix.utility:read_file;

export namespace crocofix
{

void read_file(const std::string& filename, const std::function<void(std::istream&)>& reader);

}