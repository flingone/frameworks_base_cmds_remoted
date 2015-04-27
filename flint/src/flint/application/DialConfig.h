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

#ifndef DIALCONFIG_H_
#define DIALCONFIG_H_

#include <string>
#include <map>

#include "Application.h"
#include "utils/Logging.h"

namespace flint {

class DialConfig {
public:
	struct config {
		bool useIpc_;

		bool allow_empty_post_data_;

		std::string appId_;
		std::string url_;
		int maxInactive_;

		// for testing
		void log() {
			LOG_DEBUG << "appId = " << appId_ << ", url = " << url_;
		}
	};

public:
	virtual ~DialConfig();

	static DialConfig* getInstance();

	Application* makeDialApplication(boost::asio::io_service &ioService,
			const std::string &appId, const std::string &data);

	config* getConfig(const std::string &appId);

private:
	DialConfig();

	void init();

private:
	std::map<std::string, config*> apps_;
};

} /* namespace flint */
#endif /* DIALCONFIG_H_ */
