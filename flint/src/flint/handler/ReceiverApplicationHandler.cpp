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

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <boost/algorithm/string.hpp>

#include "ReceiverApplicationHandler.h"
#include "platform/Platform.h"
#include "flint/application/Application.h"
#include "flint/application/ApplicationManager.h"
#include "utils/Logging.h"
#include "flint/session/SessionManager.h"
#include "net/NetworkManager.h"

namespace flint {

ReceiverApplicationHandler::ReceiverApplicationHandler(
		WebsocketSession *session, boost::asio::io_service& io_service,
		const std::string &appId, SessionManager& sessionManager) :
		WebsocketHandler(session), ISessionListener(), appId_(appId), ppTimer_(
				io_service), ipTimer_(io_service), sessionManager_(
				sessionManager), cacheIp_("") {
	// TODO Auto-generated constructor stub
}

ReceiverApplicationHandler::~ReceiverApplicationHandler() {
	// TODO Auto-generated destructor stub
}

/*
 * receiver app dead 3 conditions:
 *	1. call close
 *	2. websocket disconnected: (tested)
 *	3. websocket is connected but heartbeat timeout : (tested)
 */

void ReceiverApplicationHandler::close() {
	// cancel timer
	ppTimer_.cancel();
	ipTimer_.cancel();

	// super method
	WebsocketHandler::close();
}

void ReceiverApplicationHandler::onClose() {
	// notify others
	LOG_WARN << "ReceiverApplicationHandler onClose(), appid = " << appId_;

	sessionManager_.disconnectReceiver(appId_);
	sessionManager_.setSessionListender(NULL);

	// TODO: if websocket is disconnected, stop application directly???
	Application *app =
			ApplicationManager::getInstance()->getStoppingApplication();
	if (app != NULL && app->getAppId() == appId_) { // ensure app is stopped
		LOG_WARN << appId_
				<< " -> ReceiverApplicationHandler is closed!!! ensure stopping application stopped!!!";
		app->stopped();
	} else { // receiver connection is disconnected, stop receiver application
		LOG_WARN << appId_
				<< " -> ReceiverApplicationHandler is closed!!! stop application!!!";
		ApplicationManager::getInstance()->stopApplicationByAppId(appId_);
		app = ApplicationManager::getInstance()->getStoppingApplication();
		if (app != NULL) {
			app->stopped();
		}
	}

	// cancel timer
	ppTimer_.cancel();
	ipTimer_.cancel();

	// delete self
	WebsocketHandler::onClose();
}

void ReceiverApplicationHandler::onMessage(const std::string &message) {
	LOG_INFO << "ReceiverHandler received: " << message;

	// check alive application
	Application *app = ApplicationManager::getInstance()->getAliveApplication();
	if (app == NULL) {
		LOG_ERROR
				<< "No running application, close ReceiverApplicationHandler!!! appid = "
				<< appId_;
		close();
		return;
	}
	if (app->getAppId() != appId_) {
		LOG_ERROR << "Running application not match!!! appid = "
				<< app->getAppId() << ", ReceiverApplicationHandler's appid = "
				<< appId_;
		close();
		return;
	}

	rapidjson::Document d;
	d.Parse(message.c_str());
	if (d.IsNull()) {
		LOG_WARN << "parse receiver message failed! -> " << message;
		return;
	}

	// check message's item
	rapidjson::Value& appId = d["appid"];
	if (appId.IsNull()) {
		LOG_WARN << "parse receiver message: no appId! -> " << message;
		return;
	} else if (appId_ != appId.GetString()) {
		LOG_WARN << "parse receiver message: appId not match! -> " << message;
		return;
	}
	rapidjson::Value& type = d["type"];
	if (type.IsNull()) {
		LOG_WARN << "parse receiver message: no type! -> " << message;
		return;
	}

	// ensure ReceiverSession is connected while receiving a available message
	sessionManager_.touchReceiver(appId_);

	std::string typeStr = type.GetString();
	LOG_INFO << "ReceiverApplicationHandler received type: " << typeStr;
	if (boost::iequals(typeStr, "register")) {
		if (app->getUseIpc()) {
			LOG_INFO << appId_ << " is ready!!! create ReceiveSession for it.";
			sessionManager_.setSessionListender(this);
			if (!sessionManager_.createReceiver(appId_)) {
				// create receiver session failed!
				LOG_ERROR << "createReceiverSession failed ";
				close();
				return;
			}
			app->started();
			onRegister();
			asyncCheckIp();
			startHeartbeat();
			sessionManager_.touchReceiver(appId_);
		} else {
			LOG_ERROR
					<< "A un-useIpc application mustnot connect Flint Service, stop it!!! appid = "
					<< appId_;
			ApplicationManager::getInstance()->stopApplicationByAppId(appId_);
			close();
		}
	} else if (boost::iequals(typeStr, "unregister")) {
		LOG_INFO << appId_
				<< " is going to stop!!! stop application and close session!";
		ApplicationManager::getInstance()->stopApplicationByAppId(appId_);
		close();
	} else if (boost::iequals(typeStr, "additionaldata")) {
		rapidjson::Value& additionalData = d["additionaldata"];
		if (additionalData.IsNull() || !additionalData.IsObject()) {
			LOG_WARN << "parse receiver message: no additionaldata! -> "
					<< message;
			return;
		}

		std::stringstream ss;
		rapidjson::Document::MemberIterator it = additionalData.MemberBegin();
		while (true) {
			if (it == additionalData.MemberEnd()) {
				break;
			}
			std::string name = it->name.GetString();
			std::string value = it->value.GetString();
			ss << "<" << name << ">";
			ss << value;
			ss << "</" << name << ">";
			LOG_DEBUG << "get additionaldata: name = " << name << ", value = "
					<< value;
			it++;
		}
		std::string additionalDataStr = ss.str();
		LOG_DEBUG << "set additionaldata to " << appId_ << ": "
				<< additionalDataStr;
		app->setAdditionalData(additionalDataStr);
	} else if (boost::iequals(typeStr, "heartbeat")) {
		rapidjson::Value& heartbeat = d["heartbeat"];
		if (heartbeat.IsNull()) {
			LOG_WARN << "parse receiver message: no heartbeat! -> " << message;
			return;
		}
		std::string heartbeatStr = d.GetString();
		if (boost::iequals(heartbeatStr, "ping")) {
			sendHeartbeat("pong");
		} else if (boost::iequals(heartbeatStr, "pong")) {
			// ignore
		}
	} else {
		LOG_ERROR << "unknow message type : " << typeStr;
	}
}

void ReceiverApplicationHandler::startHeartbeat() {
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.String("type");
	writer.String("startHeartbeat");
	writer.String("appid");
	writer.String(appId_.c_str());
	writer.String("interval");
	writer.Int(3000);
	writer.EndObject();
	send(s.GetString());

	asyncSendPing();
}

void ReceiverApplicationHandler::asyncSendPing() {
	ppTimer_.expires_from_now(boost::posix_time::seconds(3));
	ppTimer_.async_wait(
			boost::bind(&ReceiverApplicationHandler::handleStartPing, this,
					boost::asio::placeholders::error));
}

void ReceiverApplicationHandler::handleStartPing(
		const boost::system::error_code& error) {
	if (!error) {
		sendHeartbeat("ping");
	}
	if (error != boost::asio::error::operation_aborted) {
		asyncSendPing();
	}
}

void ReceiverApplicationHandler::asyncCheckIp() {
	ipTimer_.expires_from_now(boost::posix_time::seconds(3));
	ipTimer_.async_wait(
			boost::bind(&ReceiverApplicationHandler::handleCheckIp, this,
					boost::asio::placeholders::error));
}

void ReceiverApplicationHandler::handleCheckIp(
		const boost::system::error_code& error) {
	if (!error) {
		std::string ip = NetworkManager::getInstance()->getIpAddr();
		if (!boost::iequals(cacheIp_, ip)) {
			LOG_WARN << "IP changed from [" << cacheIp_ << "] to [" << ip
					<< "]";
			onRegister();
		}
	}
	if (error != boost::asio::error::operation_aborted) {
		asyncCheckIp();
	}
}

void ReceiverApplicationHandler::sendHeartbeat(const std::string &type) {
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);

	writer.StartObject();
	writer.String("type");
	writer.String("heartbeat");
	writer.String("appid");
	writer.String(appId_.c_str());
	writer.String("heartbeat");
	writer.String(type.c_str());
	writer.EndObject();

	send(s.GetString());
}

void ReceiverApplicationHandler::onRegister() {
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);

	writer.StartObject();
	writer.String("type");
	writer.String("registerok");
	writer.String("appid");
	writer.String(appId_.c_str());

	writer.String("service_info");
	writer.StartObject();
	writer.String("ip");
	writer.StartArray();

	std::string ip = NetworkManager::getInstance()->getIpAddr();
	cacheIp_ = ip;
	LOG_WARN << "send ip to reciever: " << ip;
	LOG_WARN << "cache ip : " << cacheIp_;
	writer.String(ip.c_str());

	writer.EndArray();
	writer.String("uuid");
	writer.String(Platform::getDeviceUUID().c_str());
	writer.String("device_name");
	writer.String(Platform::getInstance()->getDeviceName().c_str());
	writer.EndObject();

	writer.EndObject();
	send(s.GetString());
}

void ReceiverApplicationHandler::onSenderConnected(const std::string &token) {
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.String("type");
	writer.String("senderconnected");
	writer.String("appid");
	writer.String(appId_.c_str());
	writer.String("token");
	writer.String(token.c_str());
	writer.EndObject();
	send(s.GetString());
}

void ReceiverApplicationHandler::onSenderDisconnected(
		const std::string &token) {
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.String("type");
	writer.String("senderdisconnected");
	writer.String("appid");
	writer.String(appId_.c_str());
	writer.String("token");
	writer.String(token.c_str());
	writer.EndObject();
	send(s.GetString());
}

/*
 * this method would be called when ReceiverSession timeout
 */
void ReceiverApplicationHandler::onReceiverDisconnected(
		const std::string &appId) {
	if (appId == appId_) {
		close();
	}
}

} /* namespace flint */
