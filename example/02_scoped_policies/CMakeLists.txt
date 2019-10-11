cmake_minimum_required (VERSION 3.6)

add_executable(${PROJECT_NAME}_example_02_scoped_policies
	"main.cpp"
)

target_link_libraries(${PROJECT_NAME}_example_02_scoped_policies
	PRIVATE ${PROJECT_NAME}
)

target_link_libraries(${PROJECT_NAME}_example_02_scoped_policies
	PRIVATE gtest_main
)