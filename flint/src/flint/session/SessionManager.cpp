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

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "SessionManager.h"
#include "utils/Logging.h"
#include "utils/UUID.h"

namespace flint {

SessionManager::SessionManager(boost::asio::io_service &ioService) :
		ioService_(ioService), receiverSession_(NULL), sessionListener_(NULL) {
	// TODO Auto-generated constructor stub
}

SessionManager::~SessionManager() {
	// TODO Auto-generated destructor stub
}

SenderSession* SessionManager::createSender(const std::string &appId) {
	std::string token = UUID::generateUUID();
	LOG_DEBUG << "create SenderSession : " << appId << " -> " << token;
	SenderSession *session = new SenderSession(ioService_, token, appId);
	session->Timeout.connect(
			boost::bind(&SessionManager::onSenderTimeout, this, ::_1));
	countSenders();
	addSender(token, session);
	countSenders();
	if (sessionListener_ != NULL) {
		sessionListener_->onSenderConnected(token);
	}
	session->triggerTimer();
	return session;
}

bool SessionManager::touchSender(const std::string &token) {
	SenderSession *session = getSender(token);
	if (session != NULL) {
		session->triggerTimer();
		return true;
	}
	return false;
}

bool SessionManager::disconnectSender(const std::string &token) {
	LOG_DEBUG << "SenderSession disconnect:" << token;
	countSenders();
	bool res = removeSender(token);
	countSenders();
	if (res) {
		if (sessionListener_ != NULL) {
			sessionListener_->onSenderDisconnected(token);
		}
		return true;
	}
	return false;
}

void SessionManager::onSenderTimeout(const std::string &token) {
	LOG_WARN << "SenderSession timeout: " << token;
	disconnectSender(token);
}

SenderSession * SessionManager::getSender(const std::string &token) {
	std::map<std::string, SenderSession *>::iterator it = senderSessions_.find(
			token);
	if (it != senderSessions_.end()) {
		return it->second;
	}
	return NULL;
}

bool SessionManager::checkSender(const std::string &token) {
	return senderSessions_.count(token) == 1;
//	return getSenderSession(token) != NULL ? true : false;
}

bool SessionManager::removeSender(const std::string &token) {
	std::map<std::string, SenderSession *>::iterator it = senderSessions_.find(
			token);
	if (it != senderSessions_.end()) {
		LOG_DEBUG << "remove SenderSession : " << token;
		senderSessions_.erase(it);
		return true;
	}
	return false;
}

bool SessionManager::addSender(const std::string &token,
		SenderSession * session) {
	if (!checkSender(token)) {
		LOG_DEBUG << "add SenderSession : " << token;
		senderSessions_.insert(
				std::pair<std::string, SenderSession*>(token, session));
		return true;
	}
	return false;
}

int SessionManager::countSenders() {
	int size = (int) senderSessions_.size();
	LOG_DEBUG << "sender sessions size is: " << size;
	return size;
}

bool SessionManager::createReceiver(const std::string &appId) {
	if (receiverSession_ != NULL) {
		LOG_WARN << "SessionManager receiver session is not NULL";
		return false;
	}
	std::string token = UUID::generateUUID();
	LOG_DEBUG << "create ReceiverSession : " << appId << " -> " << token;
	receiverSession_ = new ReceiverSession(ioService_, token, appId);
	receiverSession_->Timeout.connect(
			boost::bind(&SessionManager::onReceiverTimeout, this, ::_1));
	receiverSession_->triggerTimer();
	return true;
}

bool SessionManager::touchReceiver(const std::string &appId) {
	if (receiverSession_ != NULL) {
		receiverSession_->triggerTimer();
		return true;
	}
	return false;
}

bool SessionManager::disconnectReceiver(const std::string &appId) {
	if (receiverSession_ != NULL) {
		std::string _appId = receiverSession_->getAppId();
		if (appId == _appId) {
			receiverSession_ = NULL;
			return true;
		}
	}
	return false;
}

void SessionManager::onReceiverTimeout(const std::string &token) {
	LOG_WARN << token << " -> receiver session is timeout!!! ";
	if (receiverSession_ != NULL) {
		LOG_WARN << receiverSession_->getToken()
				<< " -> current receiver session token!!! ";
		if (token == receiverSession_->getToken()) {
			if (sessionListener_ != NULL) {
				sessionListener_->onReceiverDisconnected(
						receiverSession_->getAppId());
			}
			receiverSession_ = NULL;
		} else {
			LOG_WARN << "not match receiver session token!!! ignore ";
		}
	}
}

void SessionManager::setSessionListender(ISessionListener *sessionListener) {
	sessionListener_ = sessionListener;
	if (sessionListener_ != NULL) {
		std::map<std::string, SenderSession *>::iterator it =
				senderSessions_.begin();
		while (it != senderSessions_.end()) {
			sessionListener_->onSenderConnected(it->second->getToken());
			it++;
		}
	}
}

} /* namespace flint */
