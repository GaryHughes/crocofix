#ifndef crocofix_libcrocofix_socket_writer_hpp
#define crocofix_libcrocofix_socket_writer_hpp

#include "writer.hpp"
#include <boost/asio.hpp>

namespace crocofix
{

class socket_writer : public writer
{
public:

    socket_writer(boost::asio::ip::tcp::socket& socket);
   
    void write(message& message) override;         

private:

    void flush_pending_writes();

    boost::asio::ip::tcp::socket& m_socket;

    struct buffer_type
    {
        std::array<char, 8192> buffer;
        std::size_t offset {0};
    };

    buffer_type m_buffers[2];
    buffer_type* m_pending_buffer;
    buffer_type* m_active_buffer;

};

}

#endif