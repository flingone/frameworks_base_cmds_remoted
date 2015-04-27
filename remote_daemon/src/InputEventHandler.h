/*
 * InputEventHander.h
 *
 *  Created on: 2015-4-17
 *      Author: manson
 */

#ifndef INPUTEVENTHANDER_H_
#define INPUTEVENTHANDER_H_

#include <string>

namespace matchstick {

struct input_event {
	struct timeval time;
	unsigned short type;
	unsigned short code;
	unsigned int value;
};

class InputEventHandler {
public:
	virtual ~InputEventHandler();

private:
	InputEventHandler();
	static int sendEventInternal(const std::string& path, unsigned short type,
			unsigned short code, unsigned int value);

public:
	static int sendEvent(const std::string& path, unsigned short type,
			unsigned short code, unsigned int value);
};

}

#endif /* INPUTEVENTHANDER_H_ */
