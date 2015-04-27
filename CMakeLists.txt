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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(remoted)

INCLUDE(CMakeDependentOption)

SET(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/libs)

SET(FLINT ${CMAKE_CURRENT_SOURCE_DIR}/flint)

# sub-directories
ADD_SUBDIRECTORY(${LIBS}/boost libs/boost)

ADD_SUBDIRECTORY(${FLINT} flint)

ADD_SUBDIRECTORY(${REMOTE_DAEMON} remote_daemon)