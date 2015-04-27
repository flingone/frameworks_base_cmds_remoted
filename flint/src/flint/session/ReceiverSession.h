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

#ifndef RECEIVERSESSION_H_
#define RECEIVERSESSION_H_

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include "Session.h"
#include "SenderSession.h"

namespace flint {

class ReceiverSession: public Session {

public:
	ReceiverSession(boost::asio::io_service& io_service,
			const std::string &token, const std::string &appId);
	virtual ~ReceiverSession();

protected:
	void onTimeout(const boost::system::error_code& error);

//private:
//	void onClose();
};

} /* namespace flint */
#endif /* RECEIVERSESSION_H_ */
