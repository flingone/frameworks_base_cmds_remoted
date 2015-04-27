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

#ifndef __LOGSTREAM_H_
#define __LOGSTREAM_H_

#include <cstdio>
#include <cstring>
#include <string>
#include <boost/noncopyable.hpp>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include "utils/StringPiece.h"

namespace flint {

namespace detail {

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

//
// Use implicit_cast as a safe version of static_cast or const_cast
// for upcasting in the type hierarchy (i.e. casting a pointer to Foo
// to a pointer to SuperclassOfFoo or casting a pointer to Foo to
// a const pointer to Foo).
// When you use implicit_cast, the compiler checks that the cast is safe.
// Such explicit implicit_casts are necessary in surprisingly many
// situations where C++ demands an exact type match instead of an
// argument type convertable to a target type.
//
// The From type can be inferred, so the preferred syntax for using
// implicit_cast is the same as for static_cast etc.:
//
//   implicit_cast<ToType>(expr)
//
// implicit_cast would have been part of the C++ standard library,
// but the proposal was submitted too late.  It will probably make
// its way into the language in the future.
template<typename To, typename From>
inline To implicit_cast(From const &f) {
    return f;
}

template<int SIZE>
class FixedBuffer : boost::noncopyable {
public:
    FixedBuffer()
            : cur_(data_) {
        setCookie(cookie_start);
    }

    ~FixedBuffer() {
        setCookie(cookie_end);
    }

    void append(const char * /*restrict*/ buf, size_t len) {
        // FIXME: append partially
        if (implicit_cast<size_t>(avail()) > len) {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    const char *data() const {
        return data_;
    }

    size_t length() const {
        return static_cast<size_t>(cur_ - data_);
    }

    // write to data_ directly
    char *current() {
        return cur_;
    }

    int avail() const {
        return static_cast<int>(end() - cur_);
    }

    void add(size_t len) {
        cur_ += len;
    }

    void reset() {
        cur_ = data_;
    }

    void bzero() {
        ::bzero(data_, sizeof data_);
    }

    // for used by GDB
    const char *debugString();

    void setCookie(void (*cookie)()) {
        cookie_ = cookie;
    }

    // for used by unit test
    std::string toString() const {
        return std::string(data_, length());
    }

private:
    const char *end() const {
        return data_ + sizeof data_;
    }

    // Must be outline function for cookies.
    static void cookie_start();

    static void cookie_end();

    void (*cookie_)();

    char data_[SIZE];
    char *cur_;
};

}

class LogStream : boost::noncopyable {
    typedef LogStream self;
public:
    typedef detail::FixedBuffer<detail::kSmallBuffer> LogBuffer;

    self &operator<<(bool v) {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
    }

    self &operator<<(short);

    self &operator<<(unsigned short);

    self &operator<<(int);

    self &operator<<(unsigned int);

    self &operator<<(long);

    self &operator<<(unsigned long);

    self &operator<<(long long);

    self &operator<<(unsigned long long);

    self &operator<<(const void *);

    self &operator<<(float v) {
        *this << static_cast<double>(v);
        return *this;
    }

    self &operator<<(double);
    // self& operator<<(long double);

    self &operator<<(char v) {
        buffer_.append(&v, 1);
        return *this;
    }

    // self& operator<<(signed char);
    // self& operator<<(unsigned char);

    self &operator<<(const char *v) {
        buffer_.append(v, strlen(v));
        return *this;
    }

    self &operator<<(const std::string &v) {
        buffer_.append(v.c_str(), v.size());
        return *this;
    }

    self &operator<<(const flint::StringPiece &v) {
        buffer_.append(v.data(), v.size());
        return *this;
    }

    void append(const char *data, size_t len) {
        buffer_.append(data, len);
    }

    const LogBuffer &buffer() const {
        return buffer_;
    }

    void resetBuffer() {
        buffer_.reset();
    }

private:
    void staticCheck();

    template<typename T>
    void formatInteger(T);

    LogBuffer buffer_;

    static const int kMaxNumericSize = 32;
};

class Fmt { // : boost::noncopyable
public:
    template<typename T>
    Fmt(const char *fmt, T val);

    const char *data() const {
        return buf_;
    }

    size_t length() const {
        return length_;
    }

private:
    char buf_[32];
    size_t length_;
};

inline LogStream &operator<<(LogStream &s, const Fmt &fmt) {
    s.append(fmt.data(), fmt.length());
    return s;
}

inline LogStream &operator<<(LogStream &s, const rapidjson::Value &msg_obj) {
    rapidjson::StringBuffer sb(0, 1024 * 1024);
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    msg_obj.Accept(writer); // Accept() traverses the DOM and generates Handler events.
    s.append(sb.GetString(), sb.GetSize());
    return s;
}
}

#endif //__LOGSTREAM_H_
