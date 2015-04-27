/*
 * InputEventHander.cpp
 *
 *  Created on: 2015-4-17
 *      Author: manson
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "InputEventHandler.h"
#include "utils/Logging.h"

namespace matchstick {

InputEventHandler::InputEventHandler() {
	// TODO Auto-generated constructor stub

}

InputEventHandler::~InputEventHandler() {
	// TODO Auto-generated destructor stub
}

int InputEventHandler::sendEvent(const std::string& path, unsigned short type,
		unsigned short code, unsigned int value) {
	int ret = 0;
	ret = sendEventInternal(path, type, code, value);
	if (ret != 0) {
		return ret;
	}
	ret = sendEventInternal(path, 0, 0, 0);
	return ret;
}

int InputEventHandler::sendEventInternal(const std::string& path,
		unsigned short type, unsigned short code, unsigned int value) {
	int fd;
	int ret;
	int version;
	struct input_event event;

	fd = open(path.c_str(), O_RDWR);
	if (fd < 0) {
		LOG_ERROR << "could not open " << path.c_str() << ", "
				<< strerror(errno);
		return 1;
	}

	memset(&event, 0, sizeof(event));
	event.type = type;
	event.code = code;
	event.value = value;
	ret = write(fd, &event, sizeof(event));
	if (ret < sizeof(event)) {
		LOG_ERROR << "write event failed, " << strerror(errno);
		return -1;
	} else {
		LOG_INFO << "write event successfully! type = " << type << ", code = "
				<< code << ", value = " << value;
	}
	return 0;
}

}
