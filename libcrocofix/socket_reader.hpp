#ifndef crocofix_libcrocofix_socket_reader_hpp
#define crocofix_libcrocofix_socket_reader_hpp

#include "reader.hpp"
#include <boost/asio.hpp>

namespace crocofix
{

class socket_reader : public reader
{
public:

    socket_reader(boost::asio::ip::tcp::socket& socket);

    void read_async(reader::message_callback callback) override;

private:

    boost::asio::ip::tcp::socket& m_socket;

    std::array<char, 8192> m_read_buffer;

    size_t m_write_offset {0};
    size_t m_read_offset {0};

    message m_message;

};

}

#endif