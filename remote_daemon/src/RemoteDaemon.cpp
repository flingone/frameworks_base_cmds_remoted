/*
 * RemoteDaemon.cpp
 *
 *  Created on: 2015-4-26
 *      Author: manson
 */

#include "boost/algorithm/string.hpp"

#include "RemoteDaemon.h"
#include "InputEventHandler.h"

namespace matchstick {

RemoteDaemon::RemoteDaemon(boost::asio::io_service &ioService, int port) :
		IServer(), tcpServer_(ioService, port) {
	// TODO Auto-generated constructor stub
	tcpServer_.TcpConnection.connect(
			boost::bind(&RemoteDaemon::onSessionConnect, this, ::_1));
	tcpServer_.start();
}

RemoteDaemon::~RemoteDaemon() {
	// TODO Auto-generated destructor stub
}

void RemoteDaemon::onStart() {
	LOG_DEBUG << "RemoteDaemon onStart!!!";
}

void RemoteDaemon::onStop() {
	LOG_DEBUG << "RemoteDaemon onStop!!!";
}

void RemoteDaemon::onSessionConnect(flint::TCPSession * session) {
	TcpSessionWrapper *wrapper = new TcpSessionWrapper(session);
	wrapper->Close.connect(
			boost::bind(&RemoteDaemon::onSessionClose, this, ::_1));
	wrapper->InputEvent.connect(
			boost::bind(&RemoteDaemon::onInputEvent, this, ::_1, ::_2, ::_3));
	sessions_.push_back(wrapper);
	LOG_INFO << "add: sessions size = " << sessions_.size();
}

void RemoteDaemon::onSessionClose(const std::string &addr) {
	LOG_WARN << addr << " closed!!!";
	std::list<TcpSessionWrapper*>::iterator it = sessions_.begin();
	while (it != sessions_.end()) {
		if (boost::iequals((*it)->getAddr(), addr)) {
			sessions_.erase(it);
			break;
		} else {
			it++;
		}
	}
	LOG_INFO << "remove: sessions size = " << sessions_.size();
}

void RemoteDaemon::sendMessageToAllSessions(const std::string &message) {
	std::stringstream ss;
	ss << message.size() << ":" << message;
	std::string _message = ss.str();
//	LOG_ERROR << "send:\n" << _message;
	std::list<TcpSessionWrapper*>::iterator it = sessions_.begin();
	while (it != sessions_.end()) {
		(*it)->send(_message);
		it++;
	}
}

void RemoteDaemon::onInputEvent(int type, int code, int value) {
	LOG_WARN << "received intput event: type = " << type << ", code = " << code
			<< ", value = " << value;
	InputEventHandler::getInstance()->sendEvent((unsigned int) type,
			(unsigned int) code, value);
}

} /* namespace matchstick */
