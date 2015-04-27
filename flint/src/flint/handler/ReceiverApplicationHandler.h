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

#ifndef RECEIVERHANDLER_H_
#define RECEIVERHANDLER_H_

#include <string>
#include <boost/signals2.hpp>
#include <boost/asio.hpp>

#include "handler/WebsocketHandler.h"
#include "flint/session/SessionManager.h"
#include "flint/session/ISessionListener.h"

class SessionManager;

namespace flint {

class ReceiverApplicationHandler: public WebsocketHandler,
		public ISessionListener {
public:
	ReceiverApplicationHandler(WebsocketSession *session,
			boost::asio::io_service &io_service, const std::string &appId,
			SessionManager& sessionManager);
	virtual ~ReceiverApplicationHandler();

	virtual void close();

	virtual void onSenderConnected(const std::string &token);
	virtual void onSenderDisconnected(const std::string &token);

	virtual void onReceiverDisconnected(const std::string &appId);

public:
//	boost::signals2::signal<void()> Close;
//	boost::signals2::signal<void(const std::string &)> Message;

protected:
	void onClose();
	void onMessage(const std::string &message);

private:
	void onRegister();

	void startHeartbeat();
	void sendHeartbeat(const std::string &type);
	void asyncSendPing();
	void handleStartPing(const boost::system::error_code& error);

	void asyncCheckIp();
	void handleCheckIp(const boost::system::error_code& error);

private:
	std::string appId_;
	boost::asio::deadline_timer ppTimer_;
	boost::asio::deadline_timer ipTimer_;
	SessionManager &sessionManager_;
	std::string cacheIp_;

};

} /* namespace flint */
#endif /* RECEIVERHANDLER_H_ */
