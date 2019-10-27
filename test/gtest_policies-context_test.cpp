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

#include <gtest/gtest.h>

#include <gtest_policies/gtest_policies.h>

using namespace gtest_policies;
class DummyMonitor : public gtest_policies::detail::PolicyMonitor
{
public:
	virtual ~DummyMonitor() {}
	void Start() override {}
	bool Stop() override { return false; }
};

class DummyPolicyListener : public gtest_policies::listener::PolicyListener
{
public:
	DummyPolicyListener(gtest_policies::PolicyContext& policy)
		: PolicyListener(policy, std::make_unique<DummyMonitor>())
	{ }

	virtual ~DummyPolicyListener()
	{ }

	void OnPolicyViolation() override
	{

	}
};

TEST(PolicyContextTest, is_denied__should_return_true__if_default_constructed)
{
	PolicyContext policy;
	EXPECT_TRUE(policy.IsDenied());
}

TEST(PolicyContextTest, is_denied__should_return_false__if_constructed_as_granted)
{
	PolicyContext policy(nullptr, false);
	EXPECT_FALSE(policy.IsDenied());
}

TEST(PolicyContextTest, is_denied__should_return_true__if_deny_is_invoked)
{
	PolicyContext policy(nullptr, false);
	policy.Deny();
	EXPECT_TRUE(policy.IsDenied());
}

TEST(PolicyContextTest, is_denied__should_return_false__if_grant_is_invoked)
{
	PolicyContext policy;
	policy.Grant();
	EXPECT_FALSE(policy.IsDenied());
}

TEST(PolicyContextTest, is_violated__should_return_false__if_default_constructed)
{
	PolicyContext policy;
	EXPECT_FALSE(policy.IsViolated());
}

TEST(PolicyContextTest, set_denied__should_set_deny_state__to_given_value)
{
	PolicyContext policy;
	policy.SetDenied(false);
	EXPECT_FALSE(policy.IsDenied());
	policy.SetDenied(true);
	EXPECT_TRUE(policy.IsDenied());
}

TEST(PolicyContextTest, mark_as_violated__should_do_nothing__if_listener_is_not_set)
{
	PolicyContext policy;
	policy.MarkAsViolated();
	EXPECT_FALSE(policy.IsViolated());
}


