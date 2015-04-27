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

#include "ApplicationManager.h"
#include "utils/Logging.h"
#include "platform/Platform.h"

namespace flint {

static ApplicationManager *sApplicationManager = NULL;

ApplicationManager::ApplicationManager() :
		topApplication_(NULL), startingApplication_(NULL), stoppingApplication_(
				NULL) {
	// TODO Auto-generated constructor stub
}

ApplicationManager::~ApplicationManager() {
	// TODO Auto-generated destructor stub
}

ApplicationManager *ApplicationManager::getInstance() {
	if (sApplicationManager == NULL) {
		sApplicationManager = new ApplicationManager();
	}
	return sApplicationManager;
}

void ApplicationManager::launchApplication(Application *app) {
	if (app == NULL) {
		LOG_ERROR << "cannot launch application: null";
		return;
	}

	app->setAppState("starting");
	if (startingApplication_ != NULL) {
		if (startingApplication_->getAppId() != app->getAppId()) {
			LOG_WARN << "waiting for previous starting application -> "
					<< startingApplication_->getAppId()
					<< ", pendding application: " << app->getAppId();
			penddingApplications_.push_back(app);
		} else {
			LOG_WARN << "ignore launch request, same app is launching: "
					<< app->getAppId();
		}
	} else if (topApplication_ != NULL) {
		if (topApplication_->getAppId() != app->getAppId()) {
			LOG_WARN << topApplication_->getAppId()
					<< " is top app, it is running, pendding app: "
					<< app->getAppId() << ", and stop top application";
			penddingApplications_.push_back(app);
			stopApplication();
		} else {
			LOG_INFO << "ignore launch request, same app is running : "
					<< app->getAppId();
		}
	} else if (stoppingApplication_ != NULL) {
		LOG_WARN << stoppingApplication_->getAppId()
				<< " is stopping app, pendding app: " << app->getAppId();
		penddingApplications_.push_back(app);
	} else { // launch directly!!!
		_doLaunchApplication(app);
	}
}

void ApplicationManager::_doLaunchApplication(Application *app) {
	LOG_DEBUG << "do launch application: " << app->getAppId();
	startingApplication_ = app;
	startingApplication_->AppStarted.connect(
			boost::bind(&ApplicationManager::onAppStarted, this, ::_1));
	startingApplication_->AppStopped.connect(
			boost::bind(&ApplicationManager::onAppStopped, this, ::_1));
	startingApplication_->DoAppStop.connect(
			boost::bind(&ApplicationManager::onDoAppStop, this, ::_1));

	// TODO: check launch result!!!
	if (Platform::getInstance()->launchApplication(app->getAppInfoRaw())) { // launch success
		app->onStarting();
	} else { // launch failed
		app->stopped();
	}
}

void ApplicationManager::onDoAppStop(Application *app) {
	stopApplicationByAppId(app->getAppId());
}

void ApplicationManager::onAppStarted(Application *app) {
	app->onStarted();
	topApplication_ = app;
	startingApplication_ = NULL;
	if (penddingApplications_.size() > 0) {
		LOG_WARN << "app " << app->getAppId()
				<< " started, but there is a pendding application, continue launching!!!";
		stopApplication();
	}
}

void ApplicationManager::stopApplicationByAppId(const std::string &appId) {
	//TODO: check appID
	stopApplication();
}

void ApplicationManager::stopApplication() {
	if (stoppingApplication_ != NULL) {
		return;
	} else if (topApplication_ != NULL) {
		_doStopApplication(topApplication_);
		topApplication_ = NULL;
	} else if (startingApplication_ != NULL) {
		_doStopApplication(startingApplication_);
		startingApplication_ = NULL;
	} else {
		// no alive application
		LOG_WARN << "No running or starting app, cannot stop!!!";
		return;
	}
}

void ApplicationManager::_doStopApplication(Application *app) {
	LOG_WARN << "stop application: " << app->getAppId();
	stoppingApplication_ = app;
	Platform::getInstance()->stopApplication(app->getAppInfoRaw());
	stoppingApplication_->onStopping();
}

void ApplicationManager::onAppStopped(Application *app) {
	// TODO: check app == _stoppingApplication
	app->onStoped();
	stoppingApplication_ = NULL;
	if (penddingApplications_.size() > 0) {
		Application *app_ = penddingApplications_.front();
		penddingApplications_.erase(penddingApplications_.begin());
		launchApplication(app_);
	}
}

Application * ApplicationManager::getStoppingApplication() {
	return stoppingApplication_;
}

Application * ApplicationManager::getAliveApplication() {
	if (topApplication_) {
		return topApplication_;
	} else if (startingApplication_) {
		return startingApplication_;
	} else if (penddingApplications_.size() > 0) {
		return penddingApplications_.front();
	} else {
		return NULL;
	}
}

Application * ApplicationManager::getStartingApplication() {
	return startingApplication_;
}

} /* namespace flint */
