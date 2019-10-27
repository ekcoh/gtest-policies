///////////////////////////////////////////////////////////////////////////////
// gtest-policies
// Example 02 - Standard Output
//
// Basic Google Test example showcasing standard output policies.
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <gtest_policy/gtest_policy.h>

int main(int argc, char **argv)
{
	// Deny standard output in program scope
	gtest_policy::policies::std_cout.Deny();
	gtest_policy::policies::std_cerr.Deny();

	// Initialize Google Test as usual
	::testing::InitGoogleTest(&argc, argv);
	
	// Add policy listener to enable detection of policy violations...
	// ...or just GTEST_POLICIES_APPEND_ALL_LISTENERS for simplicity
	::testing::UnitTest::GetInstance()->listeners().Append(
		new gtest_policy::StdOutPolicyListener());
	::testing::UnitTest::GetInstance()->listeners().Append(
		new gtest_policy::StdErrPolicyListener());

	// Run Google Test as usual
	return RUN_ALL_TESTS();
}

TEST(example_02_standard_output,
	writing_to_cout_will_fail_test)
{
	gtest_policy::policies::Apply(); // Required if not using fixture
	std::cout << "Hello std::cout\n";
}

TEST(example_02_standard_output,
	writing_to_cerr_will_fail_test)
{
	gtest_policy::policies::Apply(); // Required if not using fixture
	std::cout << "Hello std::cerr\n";
}

TEST(example_02_standard_output,
	writing_to_cout_when_policy_is_granted_is_ok)
{
	gtest_policy::policies::Apply(); // Required if not using fixture
	gtest_policy::policies::std_cout.Grant(); 
	std::cout << "Hello std::cout\n";
}

TEST(example_02_standard_output,
	writing_to_cerr_when_policy_is_granted_is_ok)
{
	gtest_policy::policies::Apply(); // Required if not using fixture
	gtest_policy::policies::std_cerr.Grant();
	std::cerr << "Hello std::cerr\n";
}
