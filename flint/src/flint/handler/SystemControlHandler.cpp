/*
 * SystemControlHandler.cpp
 *
 *  Created on: 2015-3-17
 *      Author: manson
 */

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "SystemControlHandler.h"
#include "utils/Logging.h"
#include "platform/Platform.h"

namespace flint {

SystemControlHandler::SystemControlHandler(HTTPSession *session) :
		HttpHandler(session) {
	// TODO Auto-generated constructor stub
	LOG_INFO << "SystemControlHandler";

	std::string method = session->getMethod();
	if (boost::iequals(method, "POST")) {
		onPost(session);
	} else if (boost::iequals(method, "OPTIONS")) {
		onOptions(session);
	} else {
		LOG_ERROR << "unsupport method:" << method;
		response(websocketpp::http::status_code::bad_request);
	}
}

SystemControlHandler::~SystemControlHandler() {
	// TODO Auto-generated destructor stub
	LOG_INFO << "~SystemControlHandler";
}

void SystemControlHandler::onPost(HTTPSession *session) {
	LOG_DEBUG << "SystemControlHandler: POST ";

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

	std::string typeStr = type.GetString();
	if (boost::iequals(typeStr, "GET_VOLUME")) {
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.String("success");
		writer.Bool(true);
		writer.String("request_type");
		writer.String("GET_VOLUME");
		writer.String("level");
		writer.Double(Platform::getInstance()->getVolume());
		writer.EndObject();
		std::string body = s.GetString();
		std::map<std::string, std::string> headers;
		headers.insert(
				std::map<std::string, std::string>::value_type("Content-Type",
						"application/json"));
		headers.insert(
				std::map<std::string, std::string>::value_type("Content-Length",
						boost::lexical_cast<std::string>(body.length())));
		response(websocketpp::http::status_code::ok, headers, body);
	} else if (boost::iequals(typeStr, "GET_MUTED")) {

		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		writer.StartObject();
		writer.String("success");
		writer.Bool(true);
		writer.String("request_type");
		writer.String("GET_MUTED");
		writer.String("muted");
		writer.Bool(Platform::getInstance()->getMuted());
		writer.EndObject();
		std::string body = s.GetString();
		std::map<std::string, std::string> headers;
		headers.insert(
				std::map<std::string, std::string>::value_type("Content-Type",
						"application/json"));
		headers.insert(
				std::map<std::string, std::string>::value_type("Content-Length",
						boost::lexical_cast<std::string>(body.length())));
		response(websocketpp::http::status_code::ok, headers, body);
	} else if (boost::iequals(typeStr, "SET_VOLUME")) {
		rapidjson::Value& volume = d["level"];
		if (volume.IsNull()) {
			LOG_WARN << "parse post message: no level! -> " << data;
			return;
		}
		double _volume = volume.GetDouble();
		Platform::getInstance()->setVolume(_volume);
		response(websocketpp::http::status_code::ok);
	} else if (boost::iequals(typeStr, "SET_MUTED")) {
		rapidjson::Value& muted = d["muted"];
		if (muted.IsNull()) {
			LOG_WARN << "parse post message: no muted! -> " << data;
			return;
		}
		bool _muted = muted.GetBool();
		Platform::getInstance()->setMuted(_muted);
		response(websocketpp::http::status_code::ok);
	} else {
		LOG_WARN << "not support : " << data;
		response(websocketpp::http::status_code::not_found);
	}
}

void SystemControlHandler::onOptions(HTTPSession *session) {
	LOG_DEBUG << "SystemControlHandler: OPTIONS ";
	response(websocketpp::http::status_code::ok);
}

} /* namespace flint */
