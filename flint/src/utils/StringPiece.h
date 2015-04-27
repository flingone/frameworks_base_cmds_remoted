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

#ifndef __STRING_PIECE_H_
#define __STRING_PIECE_H_

#include <string>
#include <cstring>
#include <rapidjson/document.h>

namespace flint {

class StringPiece {
private:
	const char *ptr_;
	size_t length_;

public:
	// We provide non-explicit singleton constructors so users can pass
	// in a "const char*" or a "string" wherever a "StringPiece" is
	// expected.
	StringPiece() :
			ptr_(NULL), length_(0) {
	}

	StringPiece(const char *str) :
			ptr_(str), length_(strlen(ptr_)) {
	}

	StringPiece(const unsigned char *str) :
			ptr_(reinterpret_cast<const char *>(str)), length_(strlen(ptr_)) {
	}

	StringPiece(const std::string &str) :
			ptr_(str.data()), length_(static_cast<int>(str.size())) {
	}

	StringPiece(const char *offset, size_t len) :
			ptr_(offset), length_(len) {
	}

	StringPiece(const rapidjson::Value &value) :
			ptr_(value.GetString()), length_(strlen(value.GetString())) {
	}

	// data() may return a pointer to a Buffer with embedded NULs, and the
	// returned Buffer may or may not be null terminated.  Therefore it is
	// typically a mistake to pass data() to a routine that expects a NUL
	// terminated string.  Use "toString().c_str()" if you really need to do
	// this.  Or better yet, change your routine so it does not rely on NUL
	// termination.
	const char *data() const {
		return ptr_;
	}

	size_t size() const {
		return length_;
	}

	bool empty() const {
		return length_ == 0;
	}

	const char *begin() const {
		return ptr_;
	}

	const char *end() const {
		return ptr_ + length_;
	}

	void clear() {
		ptr_ = NULL;
		length_ = 0;
	}

	void set(const char *buffer, size_t len) {
		ptr_ = buffer;
		length_ = len;
	}

	void set(const char *str) {
		ptr_ = str;
		length_ = static_cast<int>(strlen(str));
	}

	void set(const void *buffer, size_t len) {
		ptr_ = reinterpret_cast<const char *>(buffer);
		length_ = len;
	}

	char operator[](int i) const {
		return ptr_[i];
	}

	void removePrefix(int n) {
		ptr_ += n;
		length_ -= n;
	}

	void removeSuffix(int n) {
		length_ -= n;
	}

	bool operator==(const StringPiece &x) const {
		return ((length_ == x.length_) && (memcmp(ptr_, x.ptr_, length_) == 0));
	}

	bool operator!=(const StringPiece &x) const {
		return ((length_ != x.length_) || (memcmp(ptr_, x.ptr_, length_) != 0));
	}

	bool operator<(const StringPiece &x) const {
		int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_);
		return ((r < 0) || ((r == 0) && (length_ < x.length_)));
	}

	bool operator<=(const StringPiece &x) const {
		int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_);
		return ((r < 0) || ((r == 0) && (length_ <= x.length_)));
	}

	bool operator>=(const StringPiece &x) const {
		int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_);
		return ((r > 0) || ((r == 0) && (length_ >= x.length_)));
	}

	bool operator>(const StringPiece &x) const {
		int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_);
		return ((r > 0) || ((r == 0) && (length_ > x.length_)));
	}

	int compare(const StringPiece &x) const {
		int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_);
		if (r == 0) {
			if (length_ < x.length_)
				r = -1;
			else if (length_ > x.length_)
				r = +1;
		}
		return r;
	}

	std::string toString() const {
		return std::string(data(), size());
	}

	void copyToString(std::string *target) const {
		target->assign(ptr_, length_);
	}

	// Does "this" start with "x"
	bool startsWith(const StringPiece &x) const {
		return ((length_ >= x.length_) && (memcmp(ptr_, x.ptr_, x.length_) == 0));
	}
};

}

std::ostream &operator<<(std::ostream &os, const flint::StringPiece &piece);

#endif //__STRING_PIECE_H_
