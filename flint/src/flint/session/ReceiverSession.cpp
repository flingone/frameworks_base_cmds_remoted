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

#include "ReceiverSession.h"
#include "platform/Platform.h"
#include "flint/application/Application.h"
#include "flint/application/ApplicationManager.h"
#include "utils/Logging.h"

namespace flint {

ReceiverSession::ReceiverSession(boost::asio::io_service& io_service,
		const std::string &token, const std::string &appId) :
		Session(io_service, token, appId) {
	// TODO Auto-generated constructor stub
//	receiverApplicationHandler_->Close.connect(
//			boost::bind(&ReceiverSession::onClose, this));
}

ReceiverSession::~ReceiverSession() {
	// TODO Auto-generated destructor stub
}

void ReceiverSession::onTimeout(const boost::system::error_code& error) {
	if (!error) {
		LOG_WARN << appId_ << " -> ReceiverSession timeout: " << token_;
	} else if (error == boost::asio::error::operation_aborted) {
		LOG_WARN << appId_ << " -> ReceiverSession timer interrupted: "
				<< token_;
	}
	Session::onTimeout(error);
}

//void ReceiverSession::onClose() {
//	LOG_WARN << "ReceiverSession onClose(), appid = " << appId_;
//	Application *app =
//			ApplicationManager::getInstance()->getStoppingApplication();
//	if (app != NULL && app->getAppId() == appId_) {
//		LOG_WARN << appId_ << " -> ReceiverSession: " << token_
//				<< "is closed!!! ensure stopping application stopped!!!";
//		app->stopped();
//	} else {
//		LOG_WARN << appId_ << " -> ReceiverSession: " << token_
//				<< "is closed!!! stop application!!!";
//		ApplicationManager::getInstance()->stopApplicationByAppId(appId_);
//	}
//	shutdown();
//}

} /* namespace flint */
