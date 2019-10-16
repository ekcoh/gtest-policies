#include <gtest/gtest.h>

#include <gtest_policy/gtest_policy.h>

class DummyPolicyListener : public gtest_policy::PolicyListener
{
public:
	DummyPolicyListener(gtest_policy::PolicyContext& policy)
		: PolicyListener(policy)
	{ }

	virtual ~DummyPolicyListener()
	{ }

	void OnPolicyViolation() override
	{

	}
	void OnPolicyChangeDuringTest(bool Deny) noexcept override
	{

	}
};

TEST(PolicyContextTest, is_denied__should_return_true__if_default_constructed)
{
	gtest_policy::PolicyContext policy;
	EXPECT_TRUE(policy.IsDenied());
}

TEST(PolicyContextTest, is_denied__should_return_false__if_constructed_as_granted)
{
	gtest_policy::PolicyContext policy(nullptr, false);
	EXPECT_FALSE(policy.IsDenied());
}

TEST(PolicyContextTest, is_denied__should_return_true__if_deny_is_invoked)
{
	gtest_policy::PolicyContext policy(nullptr, false);
	policy.Deny();
	EXPECT_TRUE(policy.IsDenied());
}

TEST(PolicyContextTest, is_denied__should_return_false__if_grant_is_invoked)
{
	gtest_policy::PolicyContext policy;
	policy.Grant();
	EXPECT_FALSE(policy.IsDenied());
}

TEST(PolicyContextTest, is_violated__should_return_false__if_default_constructed)
{
	gtest_policy::PolicyContext policy;
	EXPECT_FALSE(policy.IsViolated());
}

TEST(PolicyContextTest, set_denied__should_set_deny_state__to_given_value)
{
	gtest_policy::PolicyContext policy;
	policy.SetDenied(false);
	EXPECT_FALSE(policy.IsDenied());
	policy.SetDenied(true);
	EXPECT_TRUE(policy.IsDenied());
}

TEST(PolicyContextTest, mark_as_violated__should_do_nothing__if_listener_is_not_set)
{
	gtest_policy::PolicyContext policy;
	policy.MarkAsViolated();
	EXPECT_FALSE(policy.IsViolated());
}


