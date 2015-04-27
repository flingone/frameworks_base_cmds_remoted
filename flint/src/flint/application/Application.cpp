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

#include "Application.h"
#include "utils/Logging.h"

namespace flint {

Application::Application(boost::asio::io_service& io_service,
		const std::string &appId, const std::string &appUrl, bool useIpc,
		int maxInactive, const std::string &appInfoRaw) :
		useIpc_(useIpc), appId_(appId), appUrl_(appUrl), maxInactive_(
				maxInactive), appInfoRaw_(appInfoRaw), appState_("stopped"), additionalData_(
				""), startTimer_(io_service), stopTimer_(io_service), maxInactiveTimer_(
				io_service), wsConnected_(false) {
	// TODO Auto-generated constructor stub

	startTimeout_ = 30; // seconds
	stopTimeout_ = 10; //seconds
}

Application::~Application() {
	// TODO Auto-generated destructor stub
}

void Application::onStarting() {
	LOG_DEBUG << appId_ << " is starting!!!";
	setAppState("starting");
	if (useIpc_) {
		startTimer_.expires_from_now(boost::posix_time::seconds(startTimeout_));
		startTimer_.async_wait(
				boost::bind(&Application::onStartTimeout, this,
						boost::asio::placeholders::error));
	} else {
		started();
		if (maxInactive_ > 0) {
			LOG_DEBUG << appId_ << "'s life is " << maxInactive_ << " ms";
			maxInactiveTimer_.expires_from_now(
					boost::posix_time::milliseconds(maxInactive_));
			maxInactiveTimer_.async_wait(
					boost::bind(&Application::onMaxInactiveTimeout, this,
							boost::asio::placeholders::error));
		} else {
			// alive forever
			LOG_DEBUG << appId_ << " running forever";
		}
	}
}

void Application::onStarted() {
	LOG_DEBUG << appId_ << " is running!!!";
	setAppState("running");
}

void Application::started() {
	LOG_DEBUG << appId_ << " ensure started!!!";
	startTimer_.cancel();
	if (useIpc_) {
		wsConnected_ = true;
	}
	AppStarted(this);
}

void Application::onStopping() {
	LOG_DEBUG << appId_ << " is stopping!!!";
	setAppState("stopping");
	if (useIpc_) {
		if (wsConnected_) {
			stopTimer_.expires_from_now(
					boost::posix_time::seconds(stopTimeout_));
			stopTimer_.async_wait(
					boost::bind(&Application::onStopTimeout, this,
							boost::asio::placeholders::error));
		} else {
			LOG_WARN << "Ws is not connected yet, stop it now!!!";
			stopped();
		}
	} else {
		stopped();
	}
}

void Application::onStoped() {
	LOG_DEBUG << appId_ << " is stopped!!!";
	setAppState("stopped");
	delete this;
}

void Application::stopped() {
	LOG_DEBUG << appId_ << " ensure stopped!!!";
	stopTimer_.cancel();
	AppStopped(this);
}

void Application::onStopTimeout(const boost::system::error_code& error) {
	if (!error) {
		LOG_WARN << appId_ << " stop timeout!!! stop it!!!";
		stopped();
	}
}

void Application::onStartTimeout(const boost::system::error_code& error) {
	if (!error) {
		LOG_WARN << appId_ << " start timeout!!! stop it!!!";
//		startTimer_.cancel();
		DoAppStop(this);
	}
}

void Application::onMaxInactiveTimeout(const boost::system::error_code& error) {
	if (!error) {
		LOG_WARN << appId_ << " maxInactive timeout!!! stop it!!!";
		maxInactiveTimer_.cancel();
		DoAppStop(this);
	}
}

} /* namespace flint */
