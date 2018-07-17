# Copyright (c) 2018 Paul Ciarlo <paul.ciarlo@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.10)

project(c++forever)

include(GNUInstallDirs)

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp)
file(GLOB_RECURSE TEST_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.hpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /usr/local)

add_library(twilio++
	SHARED

	src/libmain.cpp
	src/client.cpp
	src/httpserver.cpp
	src/net.cpp
	src/sipclient.cpp
	src/http.cpp
	src/proxy.cpp
	src/sipserver.cpp
	src/httpclient.cpp
	src/server.cpp
	src/plexer.cpp
	src/message.cpp
	src/error.cpp

	${HEADER_FILES}
)
target_include_directories(twilio++ PRIVATE src)

add_executable(c++forever src/main.cpp)
target_link_libraries(c++forever twilio++)
target_include_directories(c++forever PUBLIC src)

add_executable(test++forever
	${TEST_SOURCES}
	${TEST_HEADERS}
)
add_library(gtest STATIC IMPORTED)
find_library(GTEST_LIB gtest)
add_library(gtest_main STATIC IMPORTED)
find_library(GTEST_MAIN_LIB gtest_main)
add_library(gmock STATIC IMPORTED)
find_library(GMOCK_LIB gmock)
add_library(gmock_main STATIC IMPORTED)
find_library(GMOCK_MAIN_LIB gmock_main)
target_include_directories(test++forever PUBLIC /usr/local/include)
target_link_libraries(test++forever ${GTEST_LIB} ${GTEST_MAIN_LIB} twilio++)
