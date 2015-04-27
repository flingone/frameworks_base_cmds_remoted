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

#ifndef NETWORKMANAGER_H_
#define NETWORKMANAGER_H_

#include <vector>
#include <string>

namespace flint {

class NetworkManager {
public:
	virtual ~NetworkManager();

	static NetworkManager *getInstance();

	virtual std::string getIpAddr();

#ifdef ANDROID
	void setIpAddr(const std::string ip);
#endif

protected:
	NetworkManager();

#ifndef ANDROID
protected:
	std::string getIpAddrByName(const std::string &name);
#endif

protected:
	std::string ipAddr_;

};

} /* namespace flint */
#endif /* NETWORKMANAGER_H_ */
