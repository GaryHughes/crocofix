#ifndef crocofix_libcrocofix_boost_asio_scheduler_hpp
#define crocofix_libcrocofix_boost_asio_scheduler_hpp

#include "scheduler.hpp"
#include <boost/asio.hpp>

namespace crocofix
{

class boost_asio_scheduler : public scheduler
{
public:

    boost_asio_scheduler(boost::asio::io_context& io_context);

    void run() override;

private:

    boost::asio::io_context& m_io_context;

};

}

#endif