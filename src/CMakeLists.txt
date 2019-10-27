target_sources(${PROJECT_NAME}
	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/gtest_policies-context.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/gtest_policies-listener.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/gtest_policies-alloc.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/gtest_policies-ostream.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/gtest_policies-policies.cpp"
)