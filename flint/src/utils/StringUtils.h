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

#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

#include <string>

namespace flint {

class StringUtils {
public:
	StringUtils();
	virtual ~StringUtils();

	static int FindFirstSymbol(const std::string &symbol,
			const std::string &str);

	static void ShiftStringLeft(std::string &str, int length);

	static void ShiftStringRight(std::string &str, int length);

	static void EraseString(std::string &str, int start, int length);

	static std::string HeadSubString(const std::string &str, int length);

	static std::string TailSubString(const std::string &str, int length);

	static std::string SubString(const std::string &str, int start, int length);
};

} /* namespace flint */
#endif /* STRINGUTILS_H_ */
