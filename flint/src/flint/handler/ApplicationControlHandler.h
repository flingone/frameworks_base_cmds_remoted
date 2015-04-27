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

#ifndef APPLICATIONCONTROLHANDLER_H_
#define APPLICATIONCONTROLHANDLER_H_

#include <string>
#include <map>

#include "handler/HttpHandler.h"
#include "flint/session/SessionManager.h"

namespace flint {

class ApplicationControlHandler: public HttpHandler {
public:
	ApplicationControlHandler(HTTPSession *session,
			boost::asio::io_service &ioService, SessionManager& sessionManager);
	virtual ~ApplicationControlHandler();

private:
	std::string parseAppId(const std::string &path);bool isRunningInstance(
			const std::string &path, const std::string &appId);

private:
	void onGet(HTTPSession *session, const std::string &appId);
	void onPost(HTTPSession *session, const std::string &appId);
	void onDelete(HTTPSession *session, const std::string &appId);
	void onOptions(HTTPSession *session, const std::string &appId);

	void doLaunch(HTTPSession *session, const std::string &appId,
			const std::string &appInfoRaw);
	void doStop(HTTPSession *session, const std::string &appId);

private:
	boost::asio::io_service &ioService_;
	SessionManager &sessionManager_;
};

} /* namespace flint */
#endif /* APPLICATIONCONTROLHANDLER_H_ */
