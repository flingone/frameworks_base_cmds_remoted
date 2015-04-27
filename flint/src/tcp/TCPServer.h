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

#ifndef __TCPSERVER_H_
#define __TCPSERVER_H_

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include "utils/Logging.h"
#include "comm/IServer.h"

namespace flint {

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;

class TCPSession {
public:
	TCPSession(boost::asio::io_service &io_service, sock_ptr sock,
			const std::string &addr);

	virtual ~TCPSession();

	void start();

	void close();

	void send(const std::string &message);

	const std::string &getAddr();

public:
	boost::signals2::signal<void()> Close;
	boost::signals2::signal<void(const std::string &)> Message;

private:
	void read();

private:
	void handle_send(const boost::system::error_code& error,
			std::size_t bytes_transferred);

	void handle_read(const boost::system::error_code& error, std::size_t len);

	void onClose();

private:
	boost::asio::io_service &ioService_;
	sock_ptr socket_;
	boost::array<char, 4096> read_buffer_;
	std::string addr_;
};

class TCPServer: public IServer {
public:
	TCPServer(boost::asio::io_service &ioService, int port);

	virtual ~TCPServer();

public:
	boost::signals2::signal<void(TCPSession *)> TcpConnection;

protected:
	virtual void onStart();

	virtual void onStop();

private:

	void async_accept();

	void accept_handler(sock_ptr sock, boost::system::error_code ec);

private:
	boost::asio::io_service &ioService_;
	boost::asio::ip::tcp::acceptor acceptor_;
};

}
#endif
