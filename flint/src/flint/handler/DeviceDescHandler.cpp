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

#include <map>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "DeviceDescHandler.h"
#include "utils/Logging.h"
#include "platform/Platform.h"

namespace flint {

DeviceDescHandler::DeviceDescHandler(HTTPSession *session) :
		HttpHandler(session) {
	// TODO Auto-generated constructor stub
	LOG_INFO << "DeviceDescHandler";

	Platform *platform = Platform::getInstance();

	std::string serviceURL = session->getHost() + ":9431";
	std::string friendlyName = platform->getDeviceName();
	if (boost::iequals(friendlyName, "")) {
		response(websocketpp::http::status_code::ok);
		return;
	}
	std::string manufacturer = "openflint";
	std::string modelName = platform->getModelName();
	std::string uuid = Platform::getDeviceUUID();

	std::stringstream ss;

	ss << "<?xml version=\"1.0\"?>\n";
	ss << "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n";
	ss << "  <specVersion>\n";
	ss << "    <major>1</major>\n";
	ss << "    <minor>0</minor>\n";
	ss << "  </specVersion>\n";
	ss << "  <URLBase>http://" << serviceURL << "</URLBase>\n";
	ss << "  <Host>" << session->getHost() << "</Host>\n";
	ss << "  <device>\n";
	ss
			<< "    <deviceType>urn:dial-multiscreen-org:device:dial:1</deviceType>\n";
	ss << "    <friendlyName>" << friendlyName << "</friendlyName>\n";
	ss << "    <manufacturer>" << manufacturer << "</manufacturer>\n";
	ss << "    <modelName>" << modelName << "</modelName>\n";
	ss << "    <UDN>uuid:" << uuid << "</UDN>\n";
	ss << "    <iconList>\n";
	ss << "      <icon>\n";
	ss << "        <mimetype>image/png</mimetype>\n";
	ss << "        <width>98</width>\n";
	ss << "        <height>55</height>\n";
	ss << "        <depth>32</depth>\n";
	ss << "        <url>/setup/icon.png</url>\n";
	ss << "      </icon>\n";
	ss << "    </iconList>\n";
	ss << "    <serviceList>\n";
	ss << "      <service>\n";
	ss
			<< "        <serviceType>urn:dial-multiscreen-org:service:dial:1</serviceType>\n";
	ss
			<< "        <serviceId>urn:dial-multiscreen-org:serviceId:dial</serviceId>\n";
	ss << "        <controlURL>/ssdp/notfound</controlURL>\n";
	ss << "        <eventSubURL>/ssdp/notfound</eventSubURL>\n";
	ss << "        <SCPDURL>/ssdp/notfound</SCPDURL>\n";
	ss << "      </service>\n";
	ss << "    </serviceList>\n";
	ss << "  </device>\n";
	ss << "</root>\n";
	std::string content = ss.str();

	std::map<std::string, std::string> headers;
	headers.insert(
			std::map<std::string, std::string>::value_type(
					"Access-Control-Expose-Headers", "Application-URL"));
	headers.insert(
			std::map<std::string, std::string>::value_type("Application-URL",
					"http://" + serviceURL + "/apps"));
	headers.insert(
			std::map<std::string, std::string>::value_type("Content-Type",
					"application/xml"));
	response(websocketpp::http::status_code::ok, headers, content);
}

DeviceDescHandler::~DeviceDescHandler() {
	// TODO Auto-generated destructor stub
	LOG_INFO << "~DeviceDescHandler";
}

} /* namespace flint */
