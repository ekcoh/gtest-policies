///////////////////////////////////////////////////////////////////////////////
// Example 01 - Dynamic Memory Allocation
//
// Basic Google Test example showcasing dynamic memory allocation policy.
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <gtest_policy/gtest_policy.h>

#include <vector>

int main(int argc, char **argv)
{
	// Deny dynamic memory allocation in program scope
	gtest_policy::policies::dynamic_memory_allocation.Deny();

	// Initialize Google Test as usual
	::testing::InitGoogleTest(&argc, argv);
	
	// Add policy listener to enable detection of policy violations
	::testing::UnitTest::GetInstance()->listeners().Append(
		new gtest_policy::DynamicMemoryAllocationPolicyListener());

	// Run Google Test as usual
	return RUN_ALL_TESTS();
}

TEST(example_01_dynamic_memory_allocation,
	attempting_to_allocate_via_new_will_fail_test)
{
	auto ptr = std::make_unique<int>(5);
	EXPECT_EQ(*ptr, 5);
}

TEST(example_01_dynamic_memory_allocation,
	attempting_to_allocate_via_malloc_will_fail_test)
{
	auto ptr = static_cast<int*>(malloc(sizeof(int)));
	*ptr = 5;
	EXPECT_EQ(*ptr, 5);
}

TEST(example_01_dynamic_memory_allocation,
	attempting_to_allocate_when_policy_is_temporarily_disabled_is_ok)
{
	gtest_policy::policies::dynamic_memory_allocation.Grant(); 
	auto ptr = std::make_unique<int>(3);
	gtest_policy::policies::dynamic_memory_allocation.Deny();
	EXPECT_EQ(*ptr, 3);
}

