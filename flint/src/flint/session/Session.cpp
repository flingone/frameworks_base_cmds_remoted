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

#include <boost/bind.hpp>

#include "Session.h"
#include "utils/Logging.h"

namespace flint {

Session::Session(boost::asio::io_service& io_service, const std::string &token,
		const std::string &appId) :
		timer_(io_service), token_(token), appId_(appId), closed_(false) {
	// TODO Auto-generated constructor stub

}

Session::~Session() {
	// TODO Auto-generated destructor stub
}

std::string const & Session::getToken() {
	return token_;
}

std::string const & Session::getAppId() {
	return appId_;
}

void Session::triggerTimer() {
	LOG_DEBUG << token_ << " trigger timer!";
	clearTimer();
	timer_.expires_from_now(boost::posix_time::seconds(10));
	timer_.async_wait(
			boost::bind(&Session::onTimeout, this,
					boost::asio::placeholders::error));
}

void Session::clearTimer() {
	timer_.cancel();
}

void Session::onTimeout(const boost::system::error_code& error) {
	if (!error) {
		closed_ = true;
		Timeout(token_);
	} else if (error == boost::asio::error::operation_aborted) {
		// TODO: timer is interrupted
	}

	if (closed_) {
		LOG_WARN << appId_ << " -> delete session: " << token_;
		delete this;
	}
}

} /* namespace flint */
