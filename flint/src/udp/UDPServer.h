/*
 * Copyright (C) 2013-2014, The OpenFlint Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS-IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UDPSERVER_H_
#define UDPSERVER_H_

#include <string>

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include "comm/IServer.h"

namespace flint {

class UDPServer: public IServer {
public:
	UDPServer(boost::asio::io_service &ioService, const std::string &listenIp,
			int listenPort);
	virtual ~UDPServer();

protected:
	void onStart();

	void onStop();

protected:
	virtual void parseMessage(const std::string &srcIp, int srcPort,
			const char * message, std::size_t len) = 0;

private:
	void init();

	void wait_packet();

	void handle_packet(const boost::system::error_code& /*error*/,
			std::size_t /*bytes_transferred*/);

protected:
	boost::asio::deadline_timer timer_;
	boost::asio::ip::udp::socket socket_;

	boost::asio::ip::udp::endpoint client_endpoint_;
	boost::asio::ip::udp::endpoint advertise_endpoint_;
	boost::array<char, 4096> recv_buffer_;

	std::string listenIp_;
	int listenPort_;

};

} /* namespace flint */
#endif /* UDPSERVER_H_ */
