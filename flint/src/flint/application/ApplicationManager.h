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

#ifndef APPLICATIONMANAGER_H_
#define APPLICATIONMANAGER_H_

#include <string>
#include <vector>

#include "Application.h"

namespace flint {

class ApplicationManager {
public:
	ApplicationManager();
	virtual ~ApplicationManager();

	static ApplicationManager *getInstance();

	void launchApplication(Application *app);
	void stopApplicationByAppId(const std::string &appId);
	void stopApplication();

	Application * getStoppingApplication();
	Application * getAliveApplication();
	Application * getStartingApplication();

private:
	void _doLaunchApplication(Application *app);
	void _doStopApplication(Application *app);

	void onAppStarted(Application *app);
	void onAppStopped(Application *app);
	void onDoAppStop(Application *app);

	Application* topApplication_;
	Application* startingApplication_;
	Application* stoppingApplication_;
	std::vector<Application *> penddingApplications_;
};

} /* namespace flint */
#endif /* APPLICATIONMANAGER_H_ */
