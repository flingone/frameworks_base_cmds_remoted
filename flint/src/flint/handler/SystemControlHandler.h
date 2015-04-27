/*
 * SystemControlHandler.h
 *
 *  Created on: 2015-3-17
 *      Author: manson
 */

#ifndef SYSTEMCONTROLHANDLER_H_
#define SYSTEMCONTROLHANDLER_H_

#include "handler/HttpHandler.h"

namespace flint {

class SystemControlHandler: public HttpHandler {
public:
	SystemControlHandler(HTTPSession *session);
	virtual ~SystemControlHandler();

private:
	void onPost(HTTPSession *session);

	void onOptions(HTTPSession *session);
};

} /* namespace flint */
#endif /* SYSTEMCONTROLHANDLER_H_ */
