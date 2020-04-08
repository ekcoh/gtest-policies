# Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
# This file is subject to the license terms in the LICENSE file found in the 
# root directory of this distribution.

cmake_minimum_required (VERSION 3.11)

include(FetchContent)

###################################################################################################
# project: gtest_policies
###################################################################################################

project(gtest_policies 
    VERSION 0.1.0
	HOMEPAGE_URL https://github.com/ekcoh/gtest-policies
	LANGUAGES CXX
)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UCASE)

option(${PROJECT_NAME_UCASE}_BUILD_TESTS 
	"If enabled, compile the tests." ON)
option(${PROJECT_NAME_UCASE}_BUILD_EXAMPLES 
	"If enabled, compile the examples." ON)
option(${PROJECT_NAME_UCASE}_DOWNLOAD_GTEST 
	"If enabled, download and build gtest as external project" ON)

###################################################################################################
# Download and unpack Google Test at configure time if not already available.
# If made available by parent project use that version and configuration instead.
###################################################################################################
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  if (WIN32)
    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  endif()
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

###################################################################################################
# library
###################################################################################################

add_library(${PROJECT_NAME} STATIC
	"include/gtest_policies/gtest_policies.h"
)

if(MSVC)
	# CMake appends /W3 by default, and having /W3 followed by /W4 will result in 
	# cl : Command line warning D9025 : overriding '/W3' with '/W4'.  Since this is
	# a command line warning and not a compiler warning, it cannot be suppressed except
	# by fixing the command line.
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

target_include_directories(${PROJECT_NAME} 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(src)

target_link_libraries(${PROJECT_NAME}
	PRIVATE gtest_main
)

###################################################################################################
# tests
###################################################################################################

if (${PROJECT_NAME_UCASE}_BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif(${PROJECT_NAME_UCASE}_BUILD_TESTS)

###################################################################################################
# examples
###################################################################################################

if (${PROJECT_NAME_UCASE}_BUILD_EXAMPLES)
	add_subdirectory(example)
endif(${PROJECT_NAME_UCASE}_BUILD_EXAMPLES)