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

#ifndef SESSIONMANAGER_H_
#define SESSIONMANAGER_H_

#include <string>
#include <map>

#include <boost/asio.hpp>

#include "core/Looper.h"
#include "Session.h"
#include "SenderSession.h"
#include "ReceiverSession.h"
#include "ISessionListener.h"

namespace flint {

class SessionManager {

public:
	SessionManager(boost::asio::io_service &ioService);
	virtual ~SessionManager();

	// sender
	SenderSession *createSender(const std::string &appId);

	bool touchSender(const std::string &token);

	bool checkSender(const std::string &token);

	bool disconnectSender(const std::string &token);

	// receiver
	bool createReceiver(const std::string &appId);

	bool touchReceiver(const std::string &appId);

	bool disconnectReceiver(const std::string &appId);

	void setSessionListender(ISessionListener *senderSessionListener);

private:
	SenderSession *getSender(const std::string &token);

	bool removeSender(const std::string &token);

	bool addSender(const std::string &token, SenderSession *session);

	void onSenderTimeout(const std::string &token);

	void onReceiverTimeout(const std::string &token);

	int countSenders();

private:
	boost::asio::io_service &ioService_;
	ReceiverSession *receiverSession_;
	ISessionListener *sessionListener_;

	std::map<std::string, SenderSession *> senderSessions_;
//	std::map<std::string, SenderSession *> pendingSenderSessions_;
};

} /* namespace flint */
#endif /* SESSIONMANAGER_H_ */
