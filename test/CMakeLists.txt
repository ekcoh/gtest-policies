add_executable(${PROJECT_NAME}_unit_tests
	main.cpp
	gtest_policy_test.cpp
)

target_link_libraries(${PROJECT_NAME}_unit_tests
	PRIVATE ${PROJECT_NAME}
)

enable_testing()
add_test(NAME ${PROJECT_NAME}_unit_tests COMMAND ${PROJECT_NAME}_unit_tests )

target_link_libraries(${PROJECT_NAME}_unit_tests
	PUBLIC gtest_main
)
