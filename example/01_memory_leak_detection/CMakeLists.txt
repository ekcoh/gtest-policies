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