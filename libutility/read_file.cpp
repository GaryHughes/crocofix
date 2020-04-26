#include "read_file.hpp"
#include <fstream>
#include <sstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace crocofix
{

void read_file(const std::string& filename, const std::function<void(std::istream& is)>& reader)
{
    std::ifstream file_is(filename);

    if (boost::algorithm::ends_with(filename, ".gz"))
    {
        boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
        inbuf.push(boost::iostreams::gzip_decompressor());
        inbuf.push(file_is);
        std::istream is(&inbuf);
        reader(is);
    }
    else if (boost::algorithm::ends_with(filename, ".bz2"))
    {
        boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
        inbuf.push(boost::iostreams::bzip2_decompressor());
        inbuf.push(file_is);
        std::istream is(&inbuf);
        reader(is);
    }
    else
    {
        reader(file_is);
    }
}

}