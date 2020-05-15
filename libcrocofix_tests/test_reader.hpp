#ifndef crocofix_libcrocofix_test_reader_hpp
#define crocofix_libcrocofix_test_reader_hpp

#include <libcrocofix/reader.hpp>

namespace crocofix
{

class test_reader : public reader
{
public:

    void read_async(reader::message_callback callback) override
    {

    }

};

}

#endif