#include <iostream>
#include <boost/asio.hpp>
#include <libcrocofix/session.hpp>
#include <libcrocofix/socket_reader.hpp>
#include <libcrocofix/socket_writer.hpp>
#include <libcrocofix/boost_asio_scheduler.hpp>


using boost::asio::ip::tcp;

int main(int, char**)
{
    try
    {
        int port = 5000;

        boost::asio::io_context io_context;
        crocofix::boost_asio_scheduler scheduler(io_context);
        
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

        std::cout << "waiting for initiator [*:" << port << "]";

        tcp::socket socket(io_context);
        
        acceptor.listen();
        acceptor.accept(socket);

        std::cout << "accepted initiator [" 
            << socket.remote_endpoint().address().to_string() << ":"
            << socket.remote_endpoint().port() << "]";
  
        crocofix::socket_reader reader(socket);
        crocofix::socket_writer writer(socket);
        
        crocofix::session session(reader, writer, scheduler);

        session.open();

        scheduler.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}