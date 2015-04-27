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

#include <sstream>

#include <boost/bind.hpp>

#include "TCPServer.h"
#include "utils/Logging.h"

namespace flint {

TCPServer::TCPServer(boost::asio::io_service &ioService, int port) :
		IServer(), ioService_(ioService), acceptor_(ioService,
				boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
						port)) {
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
}

TCPServer::~TCPServer() {
}

void TCPServer::onStart() {
	async_accept();
	LOG_WARN << "TCPServer is running!";
}

void TCPServer::onStop() {
	acceptor_.close();
	LOG_WARN << "TCPServer is stopped!";
}

void TCPServer::async_accept() {
	sock_ptr sock(new boost::asio::ip::tcp::socket(ioService_));
	acceptor_.async_accept(*sock,
			boost::bind(&TCPServer::accept_handler, this, sock,
					boost::asio::placeholders::error));
}

void TCPServer::accept_handler(sock_ptr sock, boost::system::error_code ec) {
	if (isRunning()) {
		std::stringstream ss;
		ss << sock->remote_endpoint().address().to_string();
		ss << ":";
		ss << sock->remote_endpoint().port();
		std::string addr = ss.str();
		LOG_INFO << "accept new connection -> " << addr;
		TcpConnection(new TCPSession(ioService_, sock, addr));
		async_accept();
	} else {
		LOG_DEBUG << "TCP server is not running, stop accept!";
	}
}

TCPSession::TCPSession(boost::asio::io_service &io_service, sock_ptr sock,
		const std::string &addr) :
		ioService_(io_service), socket_(sock), addr_(addr) {
	start();
}

TCPSession::~TCPSession() {
	LOG_DEBUG << "~TCPSession()";
}

void TCPSession::start() {
	read();
}

void TCPSession::send(const std::string &message) {
	socket_->async_send(boost::asio::buffer(message),
			boost::bind(&TCPSession::handle_send, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
}

void TCPSession::handle_send(const boost::system::error_code& error,
		std::size_t bytes_transferred) {
	if (error) {
		LOG_ERROR << "send error";
	} else {
		LOG_DEBUG << "send success";
	}
}

void TCPSession::read() {
	socket_->async_receive(boost::asio::buffer(read_buffer_),
			boost::bind(&TCPSession::handle_read, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
}

void TCPSession::handle_read(const boost::system::error_code& error,
		std::size_t len) {
	if (!error || error == boost::asio::error::message_size) {
		if (len > 0) {
			std::string message;
			message.assign(read_buffer_.data(), len);
			LOG_INFO << "received: \n" << message;
//			send("from test server!");
			Message(message);
			read();
		} else {
			// TODO: client closed???
			LOG_WARN << "read null data, client maybe closed!!! close session";
			onClose();
		}
	} else {
		// TODO: client closed???
		LOG_ERROR << "read error, client maybe closed!!! close session";
		onClose();
	}
}

void TCPSession::close() {
	try {
		socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		socket_->close();
	} catch (std::exception &e) {
		LOG_ERROR << "TCP session catch: " << e.what();
	}
}

void TCPSession::onClose() {
	LOG_DEBUG << "TCP session close!";

	try {
		socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		socket_->close();
	} catch (std::exception &e) {
		LOG_ERROR << "TCP session catch: " << e.what();
	}

	Close();

	LOG_DEBUG << "TCP session delete!";
	delete this;
}

const std::string &TCPSession::getAddr() {
	return addr_;
}

} /* namespace flint */
