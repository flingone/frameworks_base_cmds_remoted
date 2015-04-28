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

#ifndef __LOGGING_H_
#define __LOGGING_H_

#include <cstring>
#include "LogStream.h"

namespace flint {

class Logger {
public:
    enum LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    // compile time calculation of basename of source file
    class SourceFile {
    public:
        template<int N>
        inline SourceFile(const char (&arr)[N])
                : data_(arr),
                  size_(N - 1) {
            const char *slash = strrchr(data_, '/'); // builtin function
            if (slash) {
                data_ = slash + 1;
                size_ -= static_cast<int>(data_ - arr);
            }
        }

        explicit SourceFile(const char *filename)
                : data_(filename) {
            const char *slash = strrchr(filename, '/');
            if (slash) {
                data_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

        const char *data_;
        int size_;
    };

    Logger(SourceFile file, int line);

    Logger(SourceFile file, int line, LogLevel level);

    Logger(SourceFile file, int line, LogLevel level, const char *func);

    Logger(SourceFile file, int line, bool to_abort);

    ~Logger();

    LogStream &stream() {
        return impl_.stream_;
    }

#if (defined ANDROID) || (defined FFOS)
    static void enable();

    static void disable();
#endif

private:

    class Impl {
    public:
        typedef Logger::LogLevel log_level;

        Impl(log_level level, const SourceFile &file, int line);

        void finish();

        LogStream stream_;
        log_level level_;
        int line_;
        SourceFile basename_;
    };

    Impl impl_;

#if (defined ANDROID) || (defined FFOS)
    static bool DEBUG_FLAG;
#endif
};

#define LOG_TRACE \
    flint::Logger(__FILE__, __LINE__, flint::Logger::TRACE, __func__).stream()

#define LOG_DEBUG \
    flint::Logger(__FILE__, __LINE__, flint::Logger::DEBUG, __func__).stream()

#define LOG_INFO \
    flint::Logger(__FILE__, __LINE__, flint::Logger::INFO).stream()

#define LOG_WARN \
    flint::Logger(__FILE__, __LINE__, flint::Logger::WARN).stream()

#define LOG_ERROR \
    flint::Logger(__FILE__, __LINE__, flint::Logger::ERROR).stream()

#define LOG_FATAL \
    flint::Logger(__FILE__, __LINE__, flint::Logger::FATAL).stream()

#define LOG_SYSERR \
    flint::Logger(__FILE__, __LINE__, false).stream()

#define LOG_SYSFATAL \
    flint::Logger(__FILE__, __LINE__, true).stream()

}


#endif //__LOGGING_H_
