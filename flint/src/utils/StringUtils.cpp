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

#include "StringUtils.h"

namespace flint {

StringUtils::StringUtils() {
	// TODO Auto-generated constructor stub

}

StringUtils::~StringUtils() {
	// TODO Auto-generated destructor stub
}

int StringUtils::FindFirstSymbol(const std::string &symbol,
		const std::string &str) {
	return str.find_first_of(symbol);
}

void StringUtils::ShiftStringLeft(std::string &str, int length) {
	EraseString(str, 0, length);
}

void StringUtils::ShiftStringRight(std::string &str, int length) {
	int start = str.size() - length;
	EraseString(str, start, length);
}

void StringUtils::EraseString(std::string &str, int start, int length) {
	str.erase(start, length);
}

std::string StringUtils::HeadSubString(const std::string &str, int length) {
	return str.substr(0, length);
}

std::string StringUtils::TailSubString(const std::string &str, int length) {
	return str.substr(length);
}

std::string StringUtils::SubString(const std::string &str, int start,
		int length) {
	return str.substr(start, length);
}

} /* namespace flint */
