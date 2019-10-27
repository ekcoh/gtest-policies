///////////////////////////////////////////////////////////////////////////////
// gtest-policies
// Example 02 - Standard Output
//
// Basic Google Test example showcasing standard output policies.
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <gtest_policies/gtest_policies.h>

int main(int argc, char **argv)
{
	// Deny standard output in program scope
	gtest_policies::standard_output.Deny();
	gtest_policies::standard_error.Deny();

	// Initialize Google Test as usual
	::testing::InitGoogleTest(&argc, argv);
	
	// Add policy listener to enable detection of policy violations...
	// ...or just GTEST_POLICIES_APPEND_ALL_LISTENERS for simplicity
	::testing::UnitTest::GetInstance()->listeners().Append(
		new gtest_policies::listener::StdOutPolicyListener());
	::testing::UnitTest::GetInstance()->listeners().Append(
		new gtest_policies::listener::StdErrPolicyListener());

	// Run Google Test as usual
	return RUN_ALL_TESTS();
}

TEST(example_02_standard_output,
	writing_to_cout_will_fail_test)
{
	gtest_policies::Apply(); // Required if not using fixture
	std::cout << "Hello std::cout\n";
}

TEST(example_02_standard_output,
	writing_to_cerr_will_fail_test)
{
	gtest_policies::Apply(); // Required if not using fixture
	std::cout << "Hello std::cerr\n";
}

TEST(example_02_standard_output,
	writing_to_cout_when_policy_is_granted_is_ok)
{
	gtest_policies::Apply(); // Required if not using fixture
	gtest_policies::standard_output.Grant();
	std::cout << "Hello std::cout\n";
}

TEST(example_02_standard_output,
	writing_to_cerr_when_policy_is_granted_is_ok)
{
	gtest_policies::Apply(); // Required if not using fixture
	gtest_policies::standard_error.Grant();
	std::cerr << "Hello std::cerr\n";
}
