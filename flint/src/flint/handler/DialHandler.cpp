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

#include "DialHandler.h"
#include "utils/Logging.h"
#include "flint/application/DialConfig.h"
#include "flint/application/ApplicationManager.h"

namespace flint {

DialHandler::DialHandler(HTTPSession *session,
		boost::asio::io_service &ioService) :
		HttpHandler(session), ioService_(ioService) {
	// TODO Auto-generated constructor stub

	LOG_INFO << "DialHandler";

	std::string appId = parseAppId(session->getResource());
	if (appId.size() > 0) {
		std::string method = session->getMethod();
		if (boost::iequals(method, "GET")) {
			onGet(session, appId);
		} else if (boost::iequals(method, "POST")) {
			onPost(session, appId);
		} else if (boost::iequals(method, "DELETE")) {
			onDelete(session, appId);
		} else if (boost::iequals(method, "OPTIONS")) {
			onOptions(session, appId);
		} else {
			LOG_ERROR << "unsupport method:" << method;
			response(websocketpp::http::status_code::bad_request);
		}
	} else {
		LOG_ERROR << "application id illegal: " << appId;
		response(websocketpp::http::status_code::not_found);
	}
}

DialHandler::~DialHandler() {
	// TODO Auto-generated destructor stub
	LOG_INFO << "~DialHandler";
}

void DialHandler::onGet(HTTPSession *session, const std::string &appId) {
	LOG_DEBUG << "DialHandler: GET " << appId;

	std::stringstream ss;
	ss << "<?xml version='1.0' encoding='UTF-8'?>\n";
	ss << "<service xmlns='urn:dial-multiscreen-org:schemas:dial'>\n";
	ss << "  <name>" << appId << "</name>\n";
	ss << "  <options allowStop='true'/>\n";

	Application *app = ApplicationManager::getInstance()->getAliveApplication();
	if (app != NULL && (app->getAppId() == appId)) {
		ss << "  <state>" << app->getAppState() << "</state>\n";
		ss << "  <link rel=\"run\" href=\"run\"/>\n";
		ss << "  <additionalData>" << app->getAdditionalData()
				<< "</additionalData>\n";
	} else {
		ss << "  <state>" << "stopped" << "</state>\n";
	}

	ss << "</service>\n";
	std::string content = ss.str();

	std::map<std::string, std::string> headers;
	headers.insert(
			std::pair<std::string, std::string>("Content-Type",
					"application/xm"));
	//LOG_INFO << content;
	response(websocketpp::http::status_code::ok, headers, content);
}

void DialHandler::onPost(HTTPSession *session, const std::string &appId) {
	LOG_DEBUG << "DialHandler: POST " << appId;

	std::string data = session->getBody();
	LOG_DEBUG << "DialHandler: received POST " << data;

	DialConfig::config * config = DialConfig::getInstance()->getConfig(appId);
	if (config != NULL) {
		if (data.size() == 0 && !config->allow_empty_post_data_) {
			LOG_ERROR << appId << " : empty data not allowed!";
			response(websocketpp::http::status_code::bad_request);
			return;
		}
	} else {
		LOG_ERROR << appId << " : isn't support!";
		response(websocketpp::http::status_code::bad_request);
		return;
	}

	websocketpp::http::status_code::value statucCode =
			websocketpp::http::status_code::internal_server_error;
	Application *app = ApplicationManager::getInstance()->getAliveApplication();
	if (app != NULL) {
		if (app->getAppId() == appId) {
			LOG_DEBUG << appId << " is already launched! 200";
			statucCode = websocketpp::http::status_code::ok;
		} else {
			LOG_DEBUG << app->getAppId() << " is launched! stop it and launch "
					<< appId << " 201";
			doStop(session, app->getAppId());
			statucCode =
					doLaunch(session, appId, data) ?
							websocketpp::http::status_code::created :
							websocketpp::http::status_code::bad_request;
		}
	} else {
		LOG_DEBUG << appId << " is already launched! 200";
		statucCode =
				doLaunch(session, appId, data) ?
						websocketpp::http::status_code::created :
						websocketpp::http::status_code::bad_request;
	}

	response(statucCode);
}

void DialHandler::onOptions(HTTPSession *session, const std::string &appId) {
	LOG_DEBUG << "DialHandler: OPTIONS " << appId;
	response(websocketpp::http::status_code::ok);
}

void DialHandler::onDelete(HTTPSession *session, const std::string &appId) {
	LOG_DEBUG << "DialHandler: DELETE " << appId;

	websocketpp::http::status_code::value statucCode =
			websocketpp::http::status_code::internal_server_error;
	Application *app = ApplicationManager::getInstance()->getAliveApplication();
	if (app != NULL) {
		if (app->getAppId() == appId) {
			doStop(session, appId);
			statucCode = websocketpp::http::status_code::ok;
		} else {
			statucCode = websocketpp::http::status_code::bad_request;
		}
	} else {
		statucCode = websocketpp::http::status_code::not_found;
	}
	response(statucCode);
}

bool DialHandler::doLaunch(HTTPSession *session, const std::string &appId,
		const std::string &data) {
	LOG_INFO << "doLaunch: " << data;
	Application *app = DialConfig::getInstance()->makeDialApplication(
			ioService_, appId, data);
	if (app != NULL) {
		ApplicationManager::getInstance()->launchApplication(app);
		return true;
	} else {
		LOG_WARN << "make dial application failed!";
		return false;
	}
}

void DialHandler::doStop(HTTPSession *session, const std::string &appId) {
	ApplicationManager::getInstance()->stopApplicationByAppId(appId);
}

std::string DialHandler::parseAppId(const std::string &path) {
	std::string appId = path;
	boost::ierase_all(appId, "/apps/");
	boost::ierase_all(appId, "/run");
	return appId;
}

} /* namespace flint */
