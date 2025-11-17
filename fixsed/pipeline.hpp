#ifndef crocofix_fixsed_pipeline_hpp
#define crocofix_fixsed_pipeline_hpp

import crocofix;

#include "options.hpp"
#include "logging.hpp"
#include <boost/asio.hpp>

class pipeline
{
public:

	pipeline(const options& options);

	void run();

private:

	void process_messages(boost::asio::ip::tcp::socket& read_socket, 
						  boost::asio::ip::tcp::socket& write_socket, 
						  const std::string& read_label,
						  const std::string& write_label);

	void close_socket(boost::asio::ip::tcp::socket& socket);

	void log_message(const crocofix::message& message);

	const options& m_options;

};

#endif