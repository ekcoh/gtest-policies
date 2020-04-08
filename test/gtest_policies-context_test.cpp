// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

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


