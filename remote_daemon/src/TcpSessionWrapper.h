/*
 * TcpSessionWrapper.h
 *
 *  Created on: 2015-4-26
 *      Author: manson
 */

#ifndef TCPSESSIONWRAPPER_H_
#define TCPSESSIONWRAPPER_H_

#include "boost/signals2.hpp"
#include "tcp/TCPServer.h"

namespace matchstick {

class TcpSessionWrapper {
public:
	TcpSessionWrapper(flint::TCPSession *session);
	virtual ~TcpSessionWrapper();

	std::string getAddr();

	void send(const std::string &message);

public:
	boost::signals2::signal<void(const std::string&)> Close;
	boost::signals2::signal<void(int, int, int)> InputEvent;

private:
	int parseMessage(const std::string &message, std::string &outData);

	void onMessage(const std::string &message);

	void processMessage(const std::string &message);

	void onClose();

private:
	flint::TCPSession *session_;
	std::string buffer_;

};

} /* namespace matchstick */
#endif /* TCPSESSIONWRAPPER_H_ */
