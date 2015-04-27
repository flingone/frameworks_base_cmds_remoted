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

#ifndef SENDERSESSION_H_
#define SENDERSESSION_H_

#include <boost/signals2.hpp>

#include "Session.h"

namespace flint {

class SenderSession: public Session {
public:
	SenderSession(boost::asio::io_service& io_service, const std::string &token,
			const std::string &appId);

	virtual ~SenderSession();

protected:
	void onTimeout(const boost::system::error_code& error);
};

} /* namespace flint */
#endif /* SENDERSESSION_H_ */
