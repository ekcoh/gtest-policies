#ifndef GTEST_POLICY_POLICY_TEST_H
#define GTEST_POLICY_POLICY_TEST_H

#include <gtest/gtest.h>
#include <gtest/gtest-spi.h> // enables testing test failures

#include <gtest_policy/gtest_policy.h>

template<class T>
T* Create() { return new T(); }

// Fixture for testing any policy
template<class Listener>
class PolicyTest : public ::testing::Test
{
public:
	PolicyTest() : listener(Create<Listener>()), policy(listener->Policy())
	{ }

	~PolicyTest() override
	{
		delete listener;
	}

	void SetUp() override
	{
		policy.Reset(); 
	}

	::testing::UnitTest* Instance() const
	{
		return ::testing::UnitTest::GetInstance();
	}

	void GivenTestProgramStart()
	{
		listener->OnTestProgramStart(*Instance());
	}

	void GivenTestSuiteStart()
	{
		listener->OnTestSuiteStart(*Instance()->current_test_case());
	}

	void GivenTestStart()
	{
		listener->OnTestStart(*Instance()->current_test_info());
	}

	void GivenTestEnd()
	{
		listener->OnTestEnd(*Instance()->current_test_info());
	}

	void GivenTestSuiteEnd()
	{
		listener->OnTestSuiteEnd(*Instance()->current_test_case());
	}

	void GivenTestProgramEnd()
	{
		listener->OnTestProgramEnd(*Instance());
	}

	void GivenPreTestSequence()
	{
		GivenTestProgramStart();
		GivenTestSuiteStart();
		GivenTestStart();
	}

	void AssertTestEnd(bool expect_failure)
	{
		if (expect_failure)
		{
			EXPECT_NONFATAL_FAILURE(listener->OnTestEnd(
				*Instance()->current_test_info()), "Policy violation: ");
			EXPECT_TRUE(listener->IsViolated());
		}
		else
		{
			listener->OnTestEnd(
				*::testing::UnitTest::GetInstance()->current_test_info());
			EXPECT_FALSE(listener->IsViolated());
		}
	}

	void AssertPostTestSequence(bool expect_failure)
	{
		AssertTestEnd(expect_failure);
		GivenTestSuiteEnd();
		GivenTestProgramEnd();
	}

	Listener* listener;
	gtest_policy::PolicyContext& policy; 
};

TYPED_TEST_SUITE_P(PolicyTest);

TYPED_TEST_P(PolicyTest,
	policy__should_inherit_permission__if_set_on_test_program_level)
{
	policy.Grant(); // global

	GivenTestProgramStart();
	ASSERT_FALSE(policy.IsDenied());
	policy.Deny();
	GivenTestSuiteStart();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestStart();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestEnd();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestSuiteEnd();
	ASSERT_FALSE(policy.IsDenied());
	GivenTestProgramEnd();
	ASSERT_FALSE(policy.IsDenied());
}

TYPED_TEST_P(PolicyTest,
	policy__should_inherit_permission__if_set_on_test_suite_level)
{
	policy.Grant(); // global

	GivenTestProgramStart();
	ASSERT_FALSE(policy.IsDenied());
	GivenTestSuiteStart();
	ASSERT_FALSE(policy.IsDenied());
	policy.Deny();
	GivenTestStart();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestEnd();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestSuiteEnd();
	ASSERT_FALSE(policy.IsDenied());
	GivenTestProgramEnd();
	ASSERT_FALSE(policy.IsDenied());
}

TYPED_TEST_P(PolicyTest, 
	policy__should_be_reverted__if_set_on_test_level)
{
	policy.Deny(); // global

	ASSERT_TRUE(policy.IsDenied());
	GivenTestProgramStart();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestSuiteStart();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestStart();
	ASSERT_TRUE(policy.IsDenied());
	policy.Grant();
	EXPECT_FALSE(policy.IsDenied());
	GivenTestEnd();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestSuiteEnd();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestProgramEnd();
	ASSERT_TRUE(policy.IsDenied());
}

TYPED_TEST_P(PolicyTest,
	policy__should_be_reverted__if_set_on_test_suite_level)
{
	policy.Deny(); // global

	ASSERT_TRUE(policy.IsDenied());
	GivenTestProgramStart();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestSuiteStart();
	policy.Grant();
	ASSERT_FALSE(policy.IsDenied());
	GivenTestStart();
	ASSERT_FALSE(policy.IsDenied());
	GivenTestEnd();
	ASSERT_FALSE(policy.IsDenied());
	GivenTestSuiteEnd();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestProgramEnd();
	ASSERT_TRUE(policy.IsDenied());
}

REGISTER_TYPED_TEST_SUITE_P(PolicyTest, \
	policy__should_inherit_permission__if_set_on_test_program_level, \
	policy__should_inherit_permission__if_set_on_test_suite_level, \
	policy__should_be_reverted__if_set_on_test_level, \
	policy__should_be_reverted__if_set_on_test_suite_level);


#endif // GTEST_POLICY_POLICY_TEST_H