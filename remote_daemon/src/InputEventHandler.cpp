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

static InputEventHandler *sInputEventHandler = NULL;

InputEventHandler::InputEventHandler() {
	// TODO Auto-generated constructor stub
	fd_ = open("/dev/input/event0", O_RDWR);
	if (fd_ < 0) {
		LOG_ERROR << "could not open /dev/input/event0, " << strerror(errno);
		fd_ = -1;
	}
}

InputEventHandler::~InputEventHandler() {
	// TODO Auto-generated destructor stub
}

InputEventHandler* InputEventHandler::getInstance() {
	if (sInputEventHandler == NULL) {
		sInputEventHandler = new InputEventHandler();
	}
	return sInputEventHandler;
}

int InputEventHandler::sendEvent(unsigned short type, unsigned short code,
		unsigned int value) {
	int ret = 0;
	ret = sendEventInternal(type, code, value);
	if (ret != 0) {
		return ret;
	}
	ret = sendEventInternal(0, 0, 0);
	return ret;
}

int InputEventHandler::sendEventInternal(unsigned short type,
		unsigned short code, unsigned int value) {
	if (fd_ < 0) {
		LOG_ERROR << "illegal fd!!!";
		return -1;
	}
	int ret;
	int version;
	struct input_event event;

	memset(&event, 0, sizeof(event));
	event.type = type;
	event.code = code;
	event.value = value;
	ret = write(fd_, &event, sizeof(event));
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
