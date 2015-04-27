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

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "FlintServer.h"
#include "utils/Logging.h"
#include "handler/HttpHandler.h"
#include "handler/WebsocketHandler.h"
#include "flint/handler/ReceiverApplicationHandler.h"
#include "flint/handler/DeviceDescHandler.h"
#include "flint/handler/ApplicationControlHandler.h"
#include "flint/handler/SystemControlHandler.h"
#include "flint/handler/DialHandler.h"

namespace flint {

FlintServer::FlintServer(boost::asio::io_service &ioService, int httpPort) :
		IServer(), ioService_(ioService), httpServer_(ioService, httpPort), port_(
				httpPort), sessionManager_(ioService) {
	// TODO Auto-generated constructor stub
}

FlintServer::~FlintServer() {
	// TODO Auto-generated destructor stub
}

void FlintServer::onStart() {
	LOG_INFO << "FlintServer start";
	httpServer_.HttpRequest.connect(
			boost::bind(&FlintServer::onHttpRequest, this, ::_1));
	httpServer_.WebsocketConnect.connect(
			boost::bind(&FlintServer::onWebsocketConnection, this, ::_1));
	httpServer_.start();
}

void FlintServer::onStop() {
	LOG_INFO << "FlintServer stop";
	httpServer_.stop();
}

void FlintServer::onWebsocketConnection(WebsocketSession *websocketSession) {
	std::string path = websocketSession->getResource();
	LOG_DEBUG << "received WEBSOCKET connection [" << path << "]";
	if (boost::starts_with(path, "/receiver/~")) {
		std::string appId = path;
		boost::ierase_all(appId, "/receiver/");
		LOG_INFO << appId << " connect FlintServer!!!";
		new ReceiverApplicationHandler(websocketSession, ioService_, appId,
				sessionManager_);
	} else {
		LOG_WARN << "cannot handle WEBSOCKET connection [" << path << "]";
	}
}

void FlintServer::onHttpRequest(HTTPSession *httpSession) {
	std::string path = httpSession->getResource();
	LOG_DEBUG << "received HTTP request [" << path << "]";

	HttpHandler *handler = NULL;
	if (boost::iequals(path, "/setup/icon.png")) {
		// TODO:
		// handler = new IconHandler(httpSession);
	} else if (boost::iequals(path, "/ssdp/device-desc.xml")) {
		handler = new DeviceDescHandler(httpSession);
	} else if (boost::istarts_with(path, "/system/control")
			|| boost::istarts_with(path, "/system/control/")) {
		handler = new SystemControlHandler(httpSession);
	} else if (boost::istarts_with(path, "/apps/~")) {
		handler = new ApplicationControlHandler(httpSession, ioService_,
				sessionManager_);
	} else if (boost::istarts_with(path, "/apps/")) {
		handler = new DialHandler(httpSession, ioService_);
	} else {
		LOG_WARN << "cannot handle HTTP request [" << path << "]";
	}

	delete handler;
}

} /* namespace flint */
