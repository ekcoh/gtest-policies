cmake_minimum_required (VERSION 3.6)

add_executable(${PROJECT_NAME}_example_02_standard_output
	"main.cpp"
)

target_link_libraries(${PROJECT_NAME}_example_02_standard_output
	PRIVATE ${PROJECT_NAME}
)

target_link_libraries(${PROJECT_NAME}_example_02_standard_output
	PRIVATE gtest_main
)

# Uncomment to debug example tests
#enable_testing()
#add_test(NAME ${PROJECT_NAME}_example_02_standard_output 
#	COMMAND ${PROJECT_NAME}_example_02_standard_output
#)