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
#include <vector>
#include <map>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "ApplicationControlHandler.h"
#include "utils/Logging.h"
#include "platform/Platform.h"
#include "flint/application/ApplicationManager.h"
#include "flint/application/Application.h"
#include "flint/session/Session.h"

namespace flint {

ApplicationControlHandler::ApplicationControlHandler(HTTPSession *session,
		boost::asio::io_service &ioService, SessionManager& sessionManager) :
		HttpHandler(session), ioService_(ioService), sessionManager_(
				sessionManager) {
	// TODO Auto-generated constructor stub
	LOG_INFO << "ApplicationControlHandler";

	std::string appId = parseAppId(session->getResource());
	if (appId.size() > 0 && boost::istarts_with(appId, "~")) {
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

ApplicationControlHandler::~ApplicationControlHandler() {
	// TODO Auto-generated destructor stub
	LOG_INFO << "~ApplicationControlHandler";
}

void ApplicationControlHandler::onGet(HTTPSession *session,
		const std::string &appId) {
	LOG_DEBUG << "ApplicationControlHandler: GET " << appId;

	std::string authToken = session->getHeader("authorization");
	if (authToken.size() > 0) {
		LOG_DEBUG << "GET got token: " << authToken;
		if (!sessionManager_.checkSender(authToken)) {
			LOG_DEBUG << "GET token unavailable: " << authToken;
			response(websocketpp::http::status_code::bad_request);
			return;
		} else {
			sessionManager_.touchSender(authToken);
			LOG_DEBUG << "GET touch token: " << authToken;
		}
	}

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

void ApplicationControlHandler::onPost(HTTPSession *session,
		const std::string &appId) {
	LOG_DEBUG << "ApplicationControlHandler: POST " << appId;

	std::string data = session->getBody();
	if (data.size() <= 0) {
		LOG_ERROR << "received [empty] POST data!!!";
		response(websocketpp::http::status_code::bad_request);
		return;
	} else {
		LOG_DEBUG << "received POST data: [ " << data << " ]";
	}

	rapidjson::Document d;
	d.Parse(data.c_str());
	if (d.IsNull()) {
		LOG_WARN << "parse post message failed! -> " << data;
		return;
	}
	rapidjson::Value& type = d["type"];
	if (type.IsNull()) {
		LOG_WARN << "parse post message: no type! -> " << data;
		return;
	}
	rapidjson::Value& appInfo = d["app_info"];
	if (appInfo.IsNull()) {
		LOG_WARN << "parse post message: no appInfo! -> " << data;
		return;
	}

	std::string typeStr = type.GetString();
	if ((typeStr != "launch") && (typeStr != "relaunch")
			&& (typeStr != "join")) {
		LOG_ERROR << "unsupport type: " << typeStr;
		response(websocketpp::http::status_code::bad_request);
		return;
	}

	if ((typeStr == "luanch") || (typeStr == "relaunch")) {
		if (appInfo.IsNull()) {
			LOG_ERROR << "empty appInfo: ";
			response(websocketpp::http::status_code::bad_request);
			return;
		}
	}

	std::string authToken = session->getHeader("authorization");
	if (authToken.size() > 0) {
		sessionManager_.touchSender(authToken);
		LOG_DEBUG << "got token: " << authToken;
	} else {
		Session *session = sessionManager_.createSender(appId);
		authToken = session->getToken();
		LOG_DEBUG << "new token: " << authToken;
	}

	//TODO: use flint service's ip replace ${REMOTE_ADDRESS} which in appInfo.url

	websocketpp::http::status_code::value statucCode =
			websocketpp::http::status_code::internal_server_error;
	Application *app = ApplicationManager::getInstance()->getAliveApplication();
	if (app != NULL) {
		if (typeStr == "launch") {
			if (app->getAppId() == appId) {
				LOG_DEBUG << appId << " is already launched! 200";
				statucCode = websocketpp::http::status_code::ok;
			} else {
				LOG_DEBUG << app->getAppId()
						<< " is launched! stop it and launch " << appId
						<< " 201";
				rapidjson::StringBuffer buf;
				rapidjson::Writer<rapidjson::StringBuffer> w(buf);
				appInfo.Accept(w);
				doStop(session, app->getAppId());
				doLaunch(session, appId, buf.GetString());
				statucCode = websocketpp::http::status_code::created;
			}
		} else if (typeStr == "relaunch") {
			if (app->getAppId() == appId) {
				LOG_DEBUG << appId
						<< " is already launched! stop it and relaunch 201";
				rapidjson::StringBuffer buf;
				rapidjson::Writer<rapidjson::StringBuffer> w(buf);
				appInfo.Accept(w);
				doStop(session, appId);
				doLaunch(session, appId, buf.GetString());
				statucCode = websocketpp::http::status_code::created;
			} else {
				LOG_DEBUG << app->getAppId() << " is launched! cannot relauch "
						<< appId << " 400";
				response(websocketpp::http::status_code::bad_request);
				return;
			}
		} else if (typeStr == "join") {
			if (app->getAppId() == appId) {
				LOG_DEBUG << appId << " is already launched! join it 200";
				statucCode = websocketpp::http::status_code::ok;
			} else {
				LOG_DEBUG << app->getAppId() << " is launched! cannot join "
						<< appId << " 400";
				response(websocketpp::http::status_code::bad_request);
				return;
			}
		}
	} else {
		if (typeStr == "launch") {
			LOG_DEBUG << appId << " is not launched! launch it 201";
			rapidjson::StringBuffer buf;
			rapidjson::Writer<rapidjson::StringBuffer> w(buf);
			appInfo.Accept(w);
			std::string appInfoStr = buf.GetString();
			LOG_DEBUG << "appInfo :\n" << appInfoStr;
			doLaunch(session, appId, appInfoStr);
			statucCode = websocketpp::http::status_code::created;
		} else if (typeStr == "relaunch") {
			LOG_DEBUG << appId << " is not launched! cannot relaunch 400";
			response(websocketpp::http::status_code::bad_request);
			return;
		} else if (typeStr == "join") {
			LOG_DEBUG << appId << " is not launched! cannot join 400";
			response(websocketpp::http::status_code::bad_request);
			return;
		}
	}

	std::map<std::string, std::string> headers;
	headers.insert(
			std::map<std::string, std::string>::value_type("Cache-Control",
					"no-cache, must-revalidate, no-store"));
	headers.insert(
			std::map<std::string, std::string>::value_type("Content-Type",
					"application/json"));

	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.String("token");
	writer.String(authToken.c_str());
	writer.String("interval");
	writer.Int(3000);
	writer.EndObject();
	std::string body = s.GetString();
	LOG_DEBUG << "response ->\n" << body;

	response(statucCode, headers, body);
}

void ApplicationControlHandler::onDelete(HTTPSession *session,
		const std::string &appId) {
	bool isInstance = isRunningInstance(session->getResource(), appId);
	LOG_DEBUG << "ApplicationControlHandler: DELETE " << appId
			<< ", is instance = " << isInstance;
	std::string authToken = session->getHeader("authorization");
	if (authToken.size() > 0) {
		LOG_DEBUG << "DELETE got token: " << authToken;
		if (!sessionManager_.checkSender(authToken)) {
			LOG_DEBUG << "DELETE token unavailable: " << authToken;
			response(websocketpp::http::status_code::bad_request);
			return;
		}
	} else {
		LOG_DEBUG << "DELETE miss token ";
		response(websocketpp::http::status_code::bad_request);
		return;
	}

	sessionManager_.disconnectSender(authToken);
	websocketpp::http::status_code::value statucCode =
			websocketpp::http::status_code::internal_server_error;
	if (isInstance) {
		Application *app =
				ApplicationManager::getInstance()->getAliveApplication();
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
	} else {
		statucCode = websocketpp::http::status_code::ok;
	}
	response(statucCode);
}

void ApplicationControlHandler::onOptions(HTTPSession *session,
		const std::string &appId) {
	LOG_DEBUG << "ApplicationControlHandler: OPTIONS " << appId;
	response(websocketpp::http::status_code::ok);
}

void ApplicationControlHandler::doLaunch(HTTPSession *session,
		const std::string &appId, const std::string &appInfoRaw) {
	LOG_INFO << "doLaunch: " << appInfoRaw;
	rapidjson::Document appInfo;
	appInfo.Parse(appInfoRaw.c_str());
	std::string url = appInfo["url"].GetString();
	bool useIpc = appInfo["useIpc"].GetBool();
	int maxInactive = appInfo["maxInactive"].GetInt();

	Application *app = new Application(ioService_, appId, url, useIpc,
			maxInactive, appInfoRaw);
	ApplicationManager::getInstance()->launchApplication(app);
}

void ApplicationControlHandler::doStop(HTTPSession *session,
		const std::string &appId) {
	ApplicationManager::getInstance()->stopApplicationByAppId(appId);
}

std::string ApplicationControlHandler::parseAppId(const std::string &path) {
	std::string appId = path;
	boost::ierase_all(appId, "/apps/");
	boost::ierase_all(appId, "/run");
	return appId;
}

bool ApplicationControlHandler::isRunningInstance(const std::string &path,
		const std::string &appId) {
	std::string _path = path;
	boost::ierase_all(_path, "/apps/");
	boost::ierase_all(_path, appId);
	if (_path == "/run") {
		return true;
	} else {
		return false;
	}
}

} /* namespace flint */
