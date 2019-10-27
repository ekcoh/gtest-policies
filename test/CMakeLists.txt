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

add_test(NAME ${PROJECT_NAME}_unit_tests COMMAND ${PROJECT_NAME}_unit_tests )

