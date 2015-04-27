/*
 * RemoteDaemon.h
 *
 *  Created on: 2015-4-26
 *      Author: manson
 */

#ifndef REMOTEDAEMON_H_
#define REMOTEDAEMON_H_

#include <map>
#include <string>

#include "comm/IServer.h"
#include "tcp/TCPServer.h"
#include "TcpSessionWrapper.h"
#include "InputEventHandler.h"

namespace matchstick {

class RemoteDaemon: public flint::IServer {

public:
	RemoteDaemon(boost::asio::io_service &ioService, int port);
	virtual ~RemoteDaemon();

protected:
	virtual void onStart();

	virtual void onStop();

private:
	void onSessionConnect(flint::TCPSession * session);

	void onSessionClose(const std::string &addr);

	void sendMessageToAllSessions(const std::string &message);

	void onInputEvent(int type, int code, int value);

protected:
	flint::TCPServer tcpServer_;
	std::list<TcpSessionWrapper *> sessions_;
};

} /* namespace matchstick */
#endif /* REMOTEDAEMON_H_ */
