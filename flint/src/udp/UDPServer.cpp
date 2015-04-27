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

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>

#include "UDPServer.h"
#include "utils/Logging.h"
#include "net/NetworkManager.h"

namespace flint {

UDPServer::UDPServer(boost::asio::io_service &ioService,
		const std::string &listenIp, int listenPort) :
		timer_(ioService), socket_(ioService), listenIp_(listenIp), listenPort_(
				listenPort) {
	// TODO Auto-generated constructor stub
}

UDPServer::~UDPServer() {
	// TODO Auto-generated destructor stub
}

void UDPServer::init() {
//	LOG_INFO << "UDPServer init!!! -> " << listenIp_ << ":" << listenPort_;
	boost::asio::ip::address address = boost::asio::ip::address::from_string(
			"0.0.0.0");
	boost::asio::ip::udp::endpoint endpoint(address, listenPort_);

	// open
	socket_.open(endpoint.protocol());

	// reuse address, must set before binding!!!
	boost::asio::ip::udp::socket::reuse_address reuse_option(true);
	socket_.set_option(reuse_option);

	// set ttl
	boost::asio::ip::multicast::hops ttl(1);
	socket_.set_option(ttl);

	// enable loopback
	boost::asio::ip::multicast::enable_loopback loopback(true);
	socket_.set_option(loopback);

	// close quickly
	boost::asio::socket_base::linger linger_option(true, 0);
	socket_.set_option(linger_option);

	try {
		// bind
		socket_.bind(endpoint);
	} catch (boost::system::system_error &e) {
		LOG_ERROR << "UDPServer bind: " << e.what();
	}

	// join the multicast group.
	boost::asio::ip::address multicastAddress =
			boost::asio::ip::address::from_string(listenIp_);
	try {
		std::string ip = NetworkManager::getInstance()->getIpAddr();
		if (boost::iequals(ip, "")) {
//			LOG_INFO << "join group: " << multicastAddress.to_string();
			socket_.set_option(
					boost::asio::ip::multicast::join_group(multicastAddress));
		} else {
//			LOG_INFO << "join group: " << multicastAddress.to_string() << " | "
//					<< ip;
			socket_.set_option(
					boost::asio::ip::multicast::join_group(
							multicastAddress.to_v4(),
							boost::asio::ip::address::from_string(ip).to_v4()));
		}
	} catch (boost::system::system_error &e) {
		LOG_ERROR << "UDPServer join group: " << e.what();
	}
//	LOG_DEBUG << "UDPServer init!!!- end";
}

void UDPServer::onStart() {
//	LOG_INFO << "UDPServer start!!! -> " << listenIp_ << ":" << listenPort_;
	init();
	wait_packet();
}

void UDPServer::onStop() {
	socket_.close();
	timer_.cancel();
}

void UDPServer::wait_packet() {
//	LOG_DEBUG << "UDPServer wait packet!!!";
	recv_buffer_.fill(0);
	socket_.async_receive_from(boost::asio::buffer(recv_buffer_),
			client_endpoint_,
			boost::bind(&UDPServer::handle_packet, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
//	LOG_DEBUG << "UDPServer wait packet!!! end";
}

void UDPServer::handle_packet(const boost::system::error_code& error,
		std::size_t len) {
//	LOG_DEBUG << "UDPServer handle packet!!!";
	if (!error || error == boost::asio::error::message_size) {
		if (len > 0) {
//			std::string message;
//			message.assign(recv_buffer_.data(), len);
			parseMessage(client_endpoint_.address().to_string(),
					client_endpoint_.port(), recv_buffer_.data(), len);
		}
		wait_packet();
	}
//	LOG_DEBUG << "UDPServer handle packet!!! end";
}

} /* namespace flint */
