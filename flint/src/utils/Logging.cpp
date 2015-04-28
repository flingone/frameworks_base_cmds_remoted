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

#include <string>
#include <sys/errno.h>
#include "Logging.h"

#if (defined ANDROID) || (defined FFOS)

#include <android/log.h>

#define LOG_TAG "Flint"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)

#endif

using namespace flint;

namespace flint {

const char *log_levelName[Logger::NUM_LOG_LEVELS] = { "\033[;37m" "TRACE ",
		"\033[;34m" "DEBUG ", "\033[;32m" "INFO  ", "\033[;33m" "WARN  ",
		"\033[;35m" "ERROR ", "\033[;31m" "FATAL ", };

// helper class for known string length at compile time
class T {
public:
	T(const char *str, unsigned len) :
			str_(str), len_(len) {
		assert(strlen(str) == len_);
	}

	const char *str_;
	const unsigned len_;
};

inline LogStream &operator<<(LogStream &s, T v) {
	s.append(v.str_, v.len_);
	return s;
}

inline LogStream &operator<<(LogStream &s, const Logger::SourceFile &v) {
	s.append(v.data_, v.size_);
	return s;
}

}

Logger::Impl::Impl(log_level level, const SourceFile &file, int line) :
		stream_(), level_(level), line_(line), basename_(file) {
	stream_ << T(log_levelName[level], 12);
}

void Logger::Impl::finish() {
	stream_ << " - " << basename_ << ':' << line_ << "\033[0m" << '\n';
}

#if (defined ANDROID) || (defined FFOS)
	bool Logger::DEBUG_FLAG = true;

	void Logger::disable(){
		Logger::DEBUG_FLAG = false;
	}

	void Logger::enable(){
		Logger::DEBUG_FLAG = true;
	}
#endif


Logger::Logger(SourceFile file, int line) :
		impl_(INFO, file, line) {
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char *func) :
		impl_(level, file, line) {
	impl_.stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, LogLevel level) :
		impl_(level, file, line) {
}

Logger::Logger(SourceFile file, int line, bool to_abort) :
		impl_(to_abort ? FATAL : ERROR, file, line) {
}

Logger::~Logger() {
	impl_.finish();
	const LogStream::LogBuffer &buf(stream().buffer());

#if (defined ANDROID) || (defined FFOS)
	if (Logger::DEBUG_FLAG) {
		switch(impl_.level_) {
			case Logger::TRACE:
				LOGI(buf.data());
				break;
			case Logger::INFO:
				LOGI(buf.data());
				break;
			case Logger::DEBUG:
				LOGD(buf.data());
				break;
			case Logger::WARN:
				LOGW(buf.data());
				break;
			case Logger::ERROR:
				LOGE(buf.data());
				break;
			case Logger::FATAL:
				LOGF(buf.data());
				break;
			default:
				break;
		}
	}
#else
	fwrite(buf.data(), 1, (size_t) buf.length(), stdout);

	if (impl_.level_ == FATAL) {
		fflush(stdout);
		abort();
	}
#endif
}
