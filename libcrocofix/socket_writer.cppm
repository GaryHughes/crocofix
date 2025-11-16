module;

#include <boost/asio.hpp>

export module crocofix:socket_writer;

import :writer;
import :message;

export namespace crocofix
{

class socket_writer : public writer
{
public:

    socket_writer(boost::asio::ip::tcp::socket& socket);
   
    void write(message& message, int options = encode_options::standard) override;     

    void close() override;    

private:

    void flush_pending_writes();

    boost::asio::ip::tcp::socket& m_socket;
  
    static constexpr size_t buffer_size {8192};
    static constexpr size_t buffer_high_water_mark {7500};

    struct buffer_type
    {
        std::array<char, buffer_size> buffer;
        std::size_t offset {0};
    };

    buffer_type m_buffers[2];
    buffer_type* m_pending_buffer;
    buffer_type* m_active_buffer;

};

}
