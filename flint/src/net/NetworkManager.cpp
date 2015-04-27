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

#include <sys/types.h>
#include <sys/socket.h>

#ifdef FFOS
#include "utils/ifaddrs.h"
#elif ANDROID
#else
#include <ifaddrs.h>
#endif

#include <netinet/in.h>
#include <net/if.h>
#ifdef __linux__
#    include <sys/ioctl.h>
#    include <netinet/in.h>
#    include <unistd.h>
#    include <string.h>

//    Match Linux to FreeBSD
#    define AF_LINK AF_PACKET
#else
#    include <net/if_dl.h>
#endif

#include <string>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include "NetworkManager.h"
#include "utils/Logging.h"

namespace flint {

static NetworkManager *sNetworkManager = NULL;

NetworkManager::NetworkManager() :
		ipAddr_("") {
	// TODO Auto-generated constructor stub
}

NetworkManager::~NetworkManager() {
	// TODO Auto-generated destructor stub
}

NetworkManager *NetworkManager::getInstance() {
	if (sNetworkManager == NULL) {
		sNetworkManager = new NetworkManager();
	}
	return sNetworkManager;
}

std::string NetworkManager::getIpAddr() {
#ifdef ANDROID
	return ipAddr_;
#elif FFOS
	std::string ip = getIpAddrByName("wlan0");
	if (boost::iequals(ip, "")) {
		ip = getIpAddrByName("eth0");
	}
	return ip;
#elif LINUX
	std::string ip = getIpAddrByName("wlan0");
	if (boost::iequals(ip, "")) {
		ip = getIpAddrByName("eth0");
	}
	return ip;
#elif APPLE
	std::string ip = getIpAddrByName("en0");
	if (boost::iequals(ip, "")) {
		ip = getIpAddrByName("en1");
		if (boost::iequals(ip, "")) {
			ip = getIpAddrByName("en2");
			if (boost::iequals(ip, "")) {
				ip = getIpAddrByName("en3");
				if (boost::iequals(ip, "")) {
					ip = getIpAddrByName("en4");
				}
			}
		}
	}
	return ip;
#else
	return "";
#endif
}

#ifndef ANDROID
std::string NetworkManager::getIpAddrByName(const std::string &name) {
	// Head of the interface address linked list
	ifaddrs* ifap = NULL;
	int r = getifaddrs(&ifap);
	if (r != 0) {
		// Non-zero return code means an error
		LOG_ERROR << "getifaddrs error: " << r;
		return "";
	}

	ifaddrs* current = ifap;
	if (current == NULL) {
		printf("No interfaces found\n");
		LOG_ERROR << "No interfaces found!";
		return "";
	}

	std::string ip = "";
	while (current != NULL) {
		if (boost::iequals(name, current->ifa_name)
				&& current->ifa_addr->sa_family == AF_INET
				&& (current->ifa_flags & IFF_UP) /*online*/) {
			const sockaddr_in* interfaceAddress =
					reinterpret_cast<const sockaddr_in*>(current->ifa_addr);
			std::stringstream ss;
			int addr = ntohl(interfaceAddress->sin_addr.s_addr);
			ss << ((addr >> 24) & 0xFF) << ".";
			ss << ((addr >> 16) & 0xFF) << ".";
			ss << ((addr >> 8) & 0xFF) << ".";
			ss << (addr & 0xFF);
			ip = ss.str();
			break;
		}
		current = current->ifa_next;
	}
	freeifaddrs(ifap);
	ifap = NULL;

	return ip;
}
#endif

#ifdef ANDROID
void NetworkManager::setIpAddr(const std::string ip) {

	LOG_INFO << "Set IP: " << ip;
	ipAddr_ = ip;
}
#endif

} /* namespace flint */
