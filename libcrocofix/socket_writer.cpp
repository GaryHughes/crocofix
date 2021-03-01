#include "socket_writer.hpp"
#include <gsl/gsl_util>

namespace crocofix
{

socket_writer::socket_writer(boost::asio::ip::tcp::socket& socket)
:   m_socket(socket),
    m_pending_buffer(&m_buffers[0]),
    m_active_buffer(&m_buffers[1])
{
}

void socket_writer::close()
{
    m_socket.close();
}

void socket_writer::write(message& message, int options)
{
    if (m_pending_buffer->offset > buffer_high_water_mark) {
        while (m_active_buffer->offset > 0) {
            auto& io_context = static_cast<boost::asio::io_context&>(m_socket.get_executor().context()); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            io_context.poll();
        }    
    }

    auto buffer = gsl::span(&gsl::at(m_pending_buffer->buffer, m_pending_buffer->offset), 
                                     m_pending_buffer->buffer.size() - m_pending_buffer->offset);

    auto encoded_size = message.encode(buffer, options);

    if (encoded_size == 0) {
        // TODO - need to fix this to get another buffer or grow etc. Can run out of space if we send
        //        lots of messages. Particularly in a single threaded app that doesn't explicitly yield
        //        to the io_context periodically.
        throw std::runtime_error("failed to encode message");
    }

    m_pending_buffer->offset += encoded_size;

    flush_pending_writes();
}

void socket_writer::flush_pending_writes() // NOLINT(misc-no-recursion)
{
    if (m_active_buffer->offset > 0) {
        // The active buffer is still being written
        return;
    }

    std::swap(m_active_buffer, m_pending_buffer);

    auto buffer = boost::asio::buffer(&*m_active_buffer->buffer.begin(), m_active_buffer->offset);

    boost::asio::async_write(
        m_socket,
        buffer,
        [=](const boost::system::error_code& error, std::size_t /*size*/)
        {
            if (error) {
                // TODO - closed signal
                 throw boost::system::system_error(error); 
            }

            m_active_buffer->offset = 0;

            if (m_pending_buffer->offset > 0) {
                flush_pending_writes(); // NOLINT(misc-no-recursion)
            }
        }
    );
}         

}