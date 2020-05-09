#include "socket_writer.hpp"

namespace crocofix
{

socket_writer::socket_writer(boost::asio::ip::tcp::socket& socket)
:   m_socket(socket),
    m_pending_buffer(&m_buffers[0]),
    m_active_buffer(&m_buffers[1])
{
}

void socket_writer::write(message& message)
{
    auto buffer = gsl::span(&m_pending_buffer->buffer[m_pending_buffer->offset], 
                            m_pending_buffer->buffer.size() - m_pending_buffer->offset);

    auto encoded_size = message.encode(buffer);

    if (encoded_size == 0) {
        throw std::runtime_error("failed to encode message");
    }

    m_pending_buffer->offset += encoded_size;

    flush_pending_writes();
}

void socket_writer::flush_pending_writes()
{
    if (m_active_buffer->offset > 0) {
        return;
    }

    std::swap(m_active_buffer, m_pending_buffer);

    auto buffer = boost::asio::buffer(&*m_active_buffer->buffer.begin(), m_active_buffer->offset);

    boost::asio::async_write(
        m_socket,
        buffer,
        [=](const boost::system::error_code& error, std::size_t size)
        {
            if (error) {
                 throw boost::system::system_error(error); 
            }

            m_active_buffer->offset = 0;

            if (m_pending_buffer->offset > 0) {
                flush_pending_writes();
            }
        }
    );
}         

}