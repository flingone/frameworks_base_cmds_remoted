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
#include <boost/asio.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "DialConfig.h"

namespace flint {

static DialConfig *sDialConfig = NULL;

DialConfig::DialConfig() {
	// TODO Auto-generated constructor stub
	init();
}

DialConfig::~DialConfig() {
	// TODO Auto-generated destructor stub
//	std::map<std::string, config*>::iterator it = apps_.begin();
//	while (it != apps_.end()) {
//		delete it->second;
//		++it;
//	}
//	apps_.clear();
}

DialConfig* DialConfig::getInstance() {
	if (sDialConfig == NULL) {
		sDialConfig = new DialConfig();
	}
	return sDialConfig;
}

Application* DialConfig::makeDialApplication(boost::asio::io_service &ioService,
		const std::string &appId, const std::string &data) {
	config* config = getConfig(appId);
	if (config != NULL) {
		if (data == "" && !config->allow_empty_post_data_) {
			LOG_WARN << "empty post data, cannot create Application for -> "
					<< appId;
			return NULL;
		}
		std::string url = boost::replace_last_copy(config->url_, "$POST_DATA",
				data);
//		LOG_ERROR << "url = " << url;

		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		writer.StartObject();
		writer.String("url");
		writer.String(url.c_str());
		writer.String("useIpc");
		writer.Bool(config->useIpc_);
		writer.String("maxInactive");
		writer.Int(config->maxInactive_);
		writer.EndObject();
		std::string appInfoRaw = sb.GetString();
//		LOG_ERROR << "appInfoRaw = " << appInfoRaw;

		Application *app = new Application(ioService, appId, url,
				config->useIpc_, config->maxInactive_, appInfoRaw);
		return app;
	}
	return NULL;
}

void DialConfig::init() {
	//1. add YouTube
	config *youtube = new config();
	youtube->appId_ = "YouTube";
	youtube->url_ = "https://www.youtube.com/tv?$POST_DATA";
	youtube->maxInactive_ = -1;
	youtube->useIpc_ = false;
	youtube->allow_empty_post_data_ = false;

	apps_.insert(std::pair<std::string, config*>("YouTube", youtube));
}

DialConfig::config* DialConfig::getConfig(const std::string &appId) {
	std::map<std::string, config*>::iterator it = apps_.begin();
	while (it != apps_.end()) {
		if (boost::iequals(appId, it->first)) {
			return it->second;
		}
		++it;
	}
	return NULL;
}

} /* namespace flint */
