# Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
# This file is subject to the license terms in the LICENSE file found in the 
# root directory of this distribution.

include(GoogleTest)

add_executable(${PROJECT_NAME}_unit_tests
	main.cpp
	gtest_policies-alloc_test.cpp
	gtest_policies-context_test.cpp
	gtest_policies-ostream_test.cpp
)

target_link_libraries(${PROJECT_NAME}_unit_tests
	PRIVATE ${PROJECT_NAME}
	PUBLIC gtest_main
)

#add_test(NAME ${PROJECT_NAME}_unit_tests COMMAND ${PROJECT_NAME}_unit_tests )
gtest_discover_tests(${PROJECT_NAME}_unit_tests)
