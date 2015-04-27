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

#ifndef SESSION_H_
#define SESSION_H_

#include <string>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

namespace flint {

class Session {
public:
	Session(boost::asio::io_service& io_service, const std::string &token,
			const std::string &appId);
	virtual ~Session();

	void triggerTimer();

	std::string const & getToken();

	std::string const & getAppId();

public:
	boost::signals2::signal<void(const std::string &token)> Timeout;

protected:
	virtual void onTimeout(const boost::system::error_code& error);

	void clearTimer();

	boost::asio::deadline_timer timer_;

protected:
	std::string token_;

	std::string appId_;

	bool closed_;
};

} /* namespace flint */
#endif /* SESSION_H_ */
