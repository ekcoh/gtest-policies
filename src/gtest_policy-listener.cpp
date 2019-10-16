#include <gtest_policy/gtest_policy.h>

gtest_policy::PolicyListener::PolicyListener(PolicyContext& policy) noexcept : 
	::testing::TestEventListener(), 
	policy_(policy), 
	global_policy_(false), 
	stored_policy_(false), 
	violated_(false),
	in_test_scope_(false)
{ }

gtest_policy::PolicyListener::~PolicyListener() noexcept
{
	policy_.listener_ = nullptr;
}

void gtest_policy::PolicyListener::OnTestProgramStart(
	const ::testing::UnitTest& /*unit_test*/)
{ 
	policy_.listener_ = this;
}

void gtest_policy::PolicyListener::OnTestSuiteStart(
	const ::testing::TestSuite& /*test_suite*/)
{
	global_policy_ = policy_.IsDenied();
}

void gtest_policy::PolicyListener::OnTestStart(
	const ::testing::TestInfo& /*test_info*/)
{
	// Store policy setting before entering SetUp
	stored_policy_ = policy_.IsDenied();

	// Entering test scope
	in_test_scope_ = true;

	// Reset policy if previously violated in previous test
	violated_ = false;
}

void gtest_policy::PolicyListener::OnTestEnd(
	const ::testing::TestInfo& /*test_info*/ )
{
	// Only report policy violations if the test has not failed 
	// due to assertion failure
	const bool has_failure = ::testing::Test::HasFailure();
	if (!has_failure && in_test_scope_ && violated_)
		OnPolicyViolation();

	// No longer in test scope
	in_test_scope_ = false;
	
	// Restore policy setting from before invoking SetUp or test function
	RestorePolicy(stored_policy_);
}

void gtest_policy::PolicyListener::OnTestSuiteEnd(
	const ::testing::TestSuite& /*test_suite*/)
{
	RestorePolicy(global_policy_);
}

void gtest_policy::PolicyListener::OnTestProgramEnd(
	const ::testing::UnitTest& /*unit_test*/)
{ }

void gtest_policy::PolicyListener::RestorePolicy(bool Deny) noexcept
{
	if (Deny)
		policy_.Deny();
	else
		policy_.Grant();
}

void gtest_policy::PolicyListener::ReportViolation()
{
	if (in_test_scope_ && policy_.IsDenied())
	{
		violated_ = true;

		const auto has_failure = ::testing::Test::HasFailure();
		if (!has_failure)
			OnTestPartPolicyViolation();
	}
}

bool gtest_policy::PolicyListener::IsViolated() const noexcept
{
	return violated_;
}

const gtest_policy::PolicyContext& gtest_policy::PolicyListener::Policy() const noexcept
{
	return policy_;
}

gtest_policy::PolicyContext& gtest_policy::PolicyListener::Policy() noexcept
{
	return policy_;
}

