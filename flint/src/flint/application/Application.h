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

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <string>

#include <boost/signals2.hpp>
#include <boost/asio.hpp>

namespace flint {

class Application {
public:
	Application(boost::asio::io_service& io_service, const std::string &appId,
			const std::string &_appUrl, bool useIpc, int maxInactive,
			const std::string &appInfoRaw);
	virtual ~Application();

public:
	// order: onStarting() -> started() -> ApplicationManager.onAppStarted() -> onStarted()
	void onStarting();
	void onStarted();
	void started();

	// order: onStopping() -> stopped() -> ApplicationManager.onAppStopped() -> onStoped()
	void onStopping();
	void onStoped();
	void stopped();

public:
	std::string getAppId() {
		return appId_;
	}

	void setAppState(std::string appState) {
		appState_ = appState;
	}

	std::string getAppState() {
		return appState_;
	}

	void setAdditionalData(std::string additionalData) {
		additionalData_ = additionalData;
	}

	std::string getAdditionalData() {
		return additionalData_;
	}

	bool getUseIpc() {
		return useIpc_;
	}

	int getMaxInactive() {
		return maxInactive_;
	}

	std::string getAppUrl() {
		return appUrl_;
	}

	std::string getAppInfoRaw() {
		return appInfoRaw_;
	}

public:
	boost::signals2::signal<void(Application *)> AppStopped;
	boost::signals2::signal<void(Application *)> AppStarted;
	boost::signals2::signal<void(Application *)> DoAppStop;

private:
	bool useIpc_;
	std::string appId_;
	std::string appUrl_;
	int maxInactive_;
	std::string appInfoRaw_;
	std::string appState_;
	std::string additionalData_;

	bool wsConnected_;

private:
	void onStopTimeout(const boost::system::error_code& error);
	void onStartTimeout(const boost::system::error_code& error);
	void onMaxInactiveTimeout(const boost::system::error_code& error);

	boost::asio::deadline_timer startTimer_;
	boost::asio::deadline_timer stopTimer_;
	boost::asio::deadline_timer maxInactiveTimer_;

	int startTimeout_;
	int stopTimeout_;
};

} /* namespace flint */
#endif /* APPLICATION_H_ */
