# Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
# This file is subject to the license terms in the LICENSE file found in the 
# root directory of this distribution.

cmake_minimum_required (VERSION 3.6)

add_executable(${PROJECT_NAME}_example_01_memory_leak_detection
	"main.cpp"
)

target_link_libraries(${PROJECT_NAME}_example_01_memory_leak_detection
	PRIVATE ${PROJECT_NAME}
)

target_link_libraries(${PROJECT_NAME}_example_01_memory_leak_detection
	PRIVATE gtest_main
)

# Uncomment to debug example tests
#enable_testing()
#add_test(NAME ${PROJECT_NAME}_example_01_memory_leak_detection 
#	COMMAND ${PROJECT_NAME}_example_01_memory_leak_detection
#)