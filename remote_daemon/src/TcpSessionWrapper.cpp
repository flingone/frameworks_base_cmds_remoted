/*
 * TcpSessionWrapper.cpp
 *
 *  Created on: 2015-4-26
 *      Author: manson
 */

#include "boost/algorithm/string.hpp"

#include "TcpSessionWrapper.h"
#include "utils/Logging.h"
#include "utils/StringUtils.h"

namespace matchstick {

TcpSessionWrapper::TcpSessionWrapper(flint::TCPSession *session) :
		session_(session) {
	session_->Close.connect(boost::bind(&TcpSessionWrapper::onClose, this));
	session_->Message.connect(
			boost::bind(&TcpSessionWrapper::onMessage, this, ::_1));
}

TcpSessionWrapper::~TcpSessionWrapper() {
	LOG_INFO << "~TcpSessionWrapper()";
}

void TcpSessionWrapper::onMessage(const std::string &message) {
	buffer_ += message;

	std::string data = "";
	int erase = parseMessage(buffer_, data);
	while (erase > 0) {
//		LOG_DEBUG << "need to erase [" << erase << "]";
//		LOG_DEBUG << "buffer is [" << buffer_ << "]";
//		LOG_DEBUG << "data is [" << data << "]";
		flint::StringUtils::ShiftStringLeft(buffer_, erase);
//		LOG_DEBUG << "erased buffer is [" << buffer_ << "]";
		if (data != "") {
			processMessage(data);
			erase = parseMessage(buffer_, data);
		} else {
			break;
		}
	}
}

//{
//    "protocol_version":"1.0",
//    "message_type":"command",
//    "meta":{
//        "reply":false
//    },
//    "data":{
//        "command":"input_event",
//        "parameters":{
//            "type":xxx,
//            "code":xxx,
//            "value":xxx
//        }
//    }
//}
void TcpSessionWrapper::processMessage(const std::string &message) {
	LOG_INFO << session_->getAddr() << "  received\n" << message;

	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

	rapidjson::Document doc;
	doc.Parse(message.c_str());
	if (doc.IsNull()) {
		return;
	}

	if (doc["message_type"].IsNull() || !doc["message_type"].IsString()
			|| doc["data"].IsNull() || !doc["data"].IsObject()) {
		LOG_ERROR << "message missing 'message_type' or 'data'";
		return;
	}

	if (!boost::iequals("command", doc["message_type"].GetString())) {
		LOG_ERROR << "wrong 'message_type'";
		return;
	}

	rapidjson::Value &data = doc["data"];
	if (data["command"].IsNull() || !data["command"].IsString()
			|| data["parameters"].IsNull() || !data["parameters"].IsObject()) {
		LOG_ERROR << "message missing 'command' or 'parameters'";
		return;
	}

	if (!boost::iequals("input_event", data["command"].GetString())) {
		LOG_ERROR << "wrong 'command'";
		return;
	}

	rapidjson::Value &parameters = data["parameters"];
	if (parameters["type"].IsNull() || parameters["code"].IsNull()
			|| parameters["value"].IsNull()) {
		LOG_ERROR << "message missing 'type' , 'value' or 'code'";
		return;
	}

	if (!parameters["type"].IsInt() || !parameters["code"].IsInt()
			|| !parameters["value"].IsInt()) {
		LOG_ERROR << "'type' , 'value' or 'code' is not INT!";
		return;
	}

	int type = parameters["type"].GetInt();
	int code = parameters["code"].GetInt();
	int value = parameters["value"].GetInt();

	InputEvent(type, code, value);
}

void TcpSessionWrapper::onClose() {
	LOG_WARN << " session colsed!!!";
	Close(getAddr());
	session_ = NULL;
	delete this;
}

std::string TcpSessionWrapper::getAddr() {
	std::string addr = "";
	if (session_ != NULL) {
		addr = session_->getAddr();
	}
	return addr;
}

void TcpSessionWrapper::send(const std::string &message) {
	if (session_ != NULL) {
		session_->send(message);
	}
}

/**
 * parse a single message from message buffer.
 * parameters:
 * 	message: need to parse
 * 	outData: a single available message
 * return value:
 * 	int: need to erase from message buffer
 */
int TcpSessionWrapper::parseMessage(const std::string &message,
		std::string &outData) {
	int index = flint::StringUtils::FindFirstSymbol(":", message);
	if (index >= 0) {
		//parse message length
		std::string lenStr =
				flint::StringUtils::HeadSubString(message, index).c_str();
		int length = atoi(lenStr.c_str());
		if (length <= 0) { //illegal message header
			LOG_WARN << "Illegal message header : " << lenStr.c_str();
			outData = "";
			// clean all buffer
			return message.length();
		} else {
			if ((unsigned int) length <= (message.size() - lenStr.size()/* message length */
			- 1/* ":" */)) {
				//get message body
				outData = flint::StringUtils::SubString(message, index + 1,
						length);
				return (index + 1 + length);
			} else {
				// message is not complete, continue read and do not erase string
				outData = "";
				return 0;
			}
		}
	} else {
		return 0;
	}
}

} /* namespace matchstick */
