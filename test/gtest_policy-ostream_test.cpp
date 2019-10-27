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

#include "gtest_policy-policy_test.h"

using namespace gtest_policy;

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