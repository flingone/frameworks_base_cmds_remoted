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

	static InputEventHandler * getInstance();

private:
	InputEventHandler();
	int sendEventInternal(unsigned short type, unsigned short code,
			unsigned int value);

public:
	int sendEvent(unsigned short type, unsigned short code, unsigned int value);

private:
	int fd_;
};

}

#endif /* INPUTEVENTHANDER_H_ */
