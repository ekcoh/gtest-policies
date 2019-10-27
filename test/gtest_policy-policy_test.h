// 
// MIT License
// 
// Copyright(c) 2019 Håkan Sidenvall.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//
// Policy Extensions for the Google C++ Testing and Mocking Framework (Google Test).
//
// This header file defines the public API for Google Test Policy Extensions. 
// It should be included by any test program that uses Google Test Policy Extension.
//
// Code style used is same as Google Test source code to make source code blend.

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

	void GivenPolicyApplied()
	{
		policy.Apply();
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
		GivenPolicyApplied();
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
	policy.Deny(); // deny on program level
	GivenTestSuiteStart();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestStart();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestEnd();
	ASSERT_TRUE(policy.IsDenied());
	GivenTestSuiteEnd();
	ASSERT_TRUE(policy.IsDenied());
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