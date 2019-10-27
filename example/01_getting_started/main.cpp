///////////////////////////////////////////////////////////////////////////////
// gtest-policies
// Example 01 - Dynamic Memory Allocation
//
// Basic Google Test example showcasing dynamic memory allocation policy.
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <gtest_policies/gtest_policies.h>

GTEST_POLICIES_MAIN;

class example_01_getting_started : public gtest_policies::Test { };

TEST_F(example_01_getting_started, violating_std_cout)
{
	std::cout << "Hello gtest_policies";
}

TEST_F(example_01_getting_started, violating_std_cerr)
{
	std::cout << "Hello gtest_policies";
}

TEST_F(example_01_getting_started, violating_memory_alloc)
{
	auto ptr = std::make_unique<int>(5);
	EXPECT_EQ(*ptr, 5);
}

TEST_F(example_01_getting_started, no_violation)
{
	// no violation, ok
	int x = 4 + 1;
	EXPECT_EQ(x, 5);
}

