// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include "gtest_policies-policy_test.h"

using namespace gtest_policies;
using namespace gtest_policies::listener;

// Instantiate common test for a policy
INSTANTIATE_TYPED_TEST_SUITE_P(StdOutPolicyTest, \
	PolicyTest, StdOutPolicyListener);

class StdOutPolicyTest :
	public PolicyTest<StdOutPolicyListener> { };

TEST_F(StdOutPolicyTest, should_fail_test__if_denied_and_writing_to_cout)
{
	policy.Deny();
	GivenPreTestSequence();
	std::cout << "Hello";
	AssertPostTestSequence(true);
}

TEST_F(StdOutPolicyTest, should_not_fail_test__if_denied_and_flushing_cout)
{
	policy.Deny();
	GivenPreTestSequence();
	std::cout.flush();
	AssertPostTestSequence(false);
}

TEST_F(StdOutPolicyTest, should_not_fail_test__if_denied_and_writing_to_cout)
{
	policy.Grant();
	GivenPreTestSequence();
	std::cout << "Hello";
	AssertPostTestSequence(false);
}

// Instantiate common test for a policy
INSTANTIATE_TYPED_TEST_SUITE_P(StdErrPolicyTest, \
	PolicyTest, StdErrPolicyListener);

class StdErrPolicyTest :
	public PolicyTest<StdErrPolicyListener> { };

TEST_F(StdErrPolicyTest, should_fail_test__if_denied_and_writing_to_cout)
{
	policy.Deny();
	GivenPreTestSequence();
	std::cerr << "Hello";
	AssertPostTestSequence(true);
}

TEST_F(StdErrPolicyTest, should_not_fail_test__if_denied_and_flushing_cout)
{
	policy.Deny();
	GivenPreTestSequence();
	std::cerr.flush();
	AssertPostTestSequence(false);
}

TEST_F(StdErrPolicyTest, should_not_fail_test__if_denied_and_writing_to_cout)
{
	policy.Grant();
	GivenPreTestSequence();
	std::cerr << "Hello";
	AssertPostTestSequence(false);
}