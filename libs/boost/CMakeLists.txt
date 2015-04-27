#
# Copyright (C) 2013-2014, Infthink (Beijing) Technology Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS-IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 2.8)

project(boost)

include(CMakeDependentOption)

# build type -------------------------------------------------------------------

#message("***** libs/boost/CMakeLists.txt dump status *****")
#message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "")
    #message(STATUS "CMAKE_C_FLAGS=${CMAKE_C_FLAGS}")
    #message(STATUS "CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")
endif(CMAKE_BUILD_TYPE STREQUAL "")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    #message(STATUS "CMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}")
    #message(STATUS "CMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    #message(STATUS "CMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}")
    #message(STATUS "CMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# options ----------------------------------------------------------------------

# variables --------------------------------------------------------------------

## boost-include
set(boost-include 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

## boost-atomic-src
set(boost-atomic-src
    libs/atomic/src/lockpool.cpp
)

## boost-system-src
set(boost-system-src
    libs/system/src/error_code.cpp
)

## boost-thread-src
set(boost-thread-src
    libs/thread/src/future.cpp
    libs/thread/src/pthread/once.cpp
    libs/thread/src/pthread/thread.cpp
)

## boost-filesystem-src
set(boost-filesystem-src
    libs/filesystem/src/codecvt_error_category.cpp
    libs/filesystem/src/operations.cpp
    libs/filesystem/src/path.cpp
    libs/filesystem/src/path_traits.cpp
    libs/filesystem/src/portability.cpp
    libs/filesystem/src/unique_path.cpp
    libs/filesystem/src/utf8_codecvt_facet.cpp
    libs/filesystem/src/windows_file_codecvt.cpp
)

## boost-regex-src
set(boost-regex-src
    libs/regex/src/c_regex_traits.cpp
    libs/regex/src/cpp_regex_traits.cpp
	libs/regex/src/cregex.cpp
	libs/regex/src/fileiter.cpp
	libs/regex/src/icu.cpp
	libs/regex/src/instances.cpp
	libs/regex/src/internals.hpp
	libs/regex/src/posix_api.cpp
	libs/regex/src/regex_debug.cpp
	libs/regex/src/regex_raw_buffer.cpp
	libs/regex/src/regex_traits_defaults.cpp
	libs/regex/src/regex.cpp
	libs/regex/src/static_mutex.cpp
	libs/regex/src/usinstances.cpp
	libs/regex/src/w32_regex_traits.cpp
	libs/regex/src/wc_regex_traits.cpp
	libs/regex/src/wide_posix_api.cpp
	libs/regex/src/winstances.cpp
)

## boost-libs
set(boost-libs)

## boost-linklibs
set(boost-linklibs)

## boost-cxx11-cflags
#set(boost-cxx11-cflags
#    --std=c++0x
#)

## boost-cflags
if(UNIX)
    set(boost-cflags
        -Wall
        -frtti
        -fPIC
        -fexceptions
        -D_FILE_OFFSET_BITS=64
        -DBOOST_THREAD_POSIX=1
        -DBOOST_ALL_NO_LIB=1
        -DBOOST_IOSTREAMS_USE_DEPRECATED=1
        ${boost-cxx11-cflags}
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(libnode-cflags
            ${libnode-cflags}
            -Wno-unused-variable
            -Wno-unused-parameter
            -Wno-unused-private-field
        )
    endif(CMAKE_BUILD_TYPE STREQUAL "Release")

endif(UNIX)

string(REPLACE ";" " " boost-cflags "${boost-cflags}")

## boost-test-cflags
string(REPLACE ";" " " boost-test-cflags "${boost-test-cflags}")

# build libraries --------------------------------------------------------------

include_directories(${boost-include})

link_directories(${CMAKE_BINARY_DIR})

## libboost-atomic
add_library(boost-atomic STATIC ${boost-atomic-src})

target_link_libraries(boost-atomic ${boost-libs})

set_target_properties(boost-atomic PROPERTIES COMPILE_FLAGS "${boost-cflags}")

## libboost-system
add_library(boost-system STATIC ${boost-system-src})

target_link_libraries(boost-system ${boost-libs})

set_target_properties(boost-system PROPERTIES COMPILE_FLAGS "${boost-cflags}")

## libboost-thread
add_library(boost-thread STATIC ${boost-thread-src})

target_link_libraries(boost-thread ${boost-libs})

set_target_properties(boost-thread PROPERTIES COMPILE_FLAGS "${boost-cflags}")

## libboost-filesystem
add_library(boost-filesystem STATIC ${boost-filesystem-src})	

target_link_libraries(boost-filesystem ${boost-libs})

set_target_properties(boost-filesystem PROPERTIES COMPILE_FLAGS "${boost-cflags}")

## libboost-regex
add_library(boost-regex STATIC ${boost-regex-src})

target_link_libraries(boost-regex ${boost-libs})

set_target_properties(boost-regex PROPERTIES COMPILE_FLAGS "${boost-cflags}")