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

#include "IServer.h"
#include "utils/Logging.h"

namespace flint {

IServer::IServer() :
		isRunning_(false) {
	// TODO Auto-generated constructor stub

}

IServer::~IServer() {
	// TODO Auto-generated destructor stub
}

void IServer::start() {
	if (isRunning_) {
		LOG_WARN << "Server is already running!!!";
		return;
	} else {
		isRunning_ = true;
		onStart();
	}
}

void IServer::stop() {
	if (!isRunning_) {
		LOG_WARN << "Server not running skip stop";
		return;
	} else {
		isRunning_ = false;
		onStop();
	}
}

bool IServer::isRunning() {
	return isRunning_;
}

} /* namespace flint */
