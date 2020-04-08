// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include <gtest_policies/gtest_policies.h>

gtest_policies::listener::PolicyListener::PolicyListener(
	PolicyContext& policy, std::unique_ptr<detail::PolicyMonitor>&& monitor) noexcept : 
	::testing::TestEventListener(), 
	policy_(policy), 
	monitor_(std::move(monitor)),
	global_policy_(false),
	program_policy_(false), 
	stored_policy_(false), 
	violated_(false),
	in_test_scope_(false),
	applied_(false)
{ }

gtest_policies::listener::PolicyListener::~PolicyListener() noexcept
{
	policy_.listener_ = nullptr;
}

void gtest_policies::listener::PolicyListener::OnTestProgramStart(
	const ::testing::UnitTest& /*unit_test*/)
{ 
	policy_.listener_ = this;
	global_policy_ = policy_.IsDenied();
}

void gtest_policies::listener::PolicyListener::OnTestSuiteStart(
	const ::testing::TestSuite& /*test_suite*/)
{
	program_policy_ = policy_.IsDenied();
}

void gtest_policies::listener::PolicyListener::Apply()
{
	if (!applied_)
	{
		applied_ = true;
		if (Policy().IsDenied())
			monitor_->Start();
	}
}

void gtest_policies::listener::PolicyListener::OnTestStart(
	const ::testing::TestInfo& /*test_info*/)
{
	// Store policy setting before entering SetUp
	stored_policy_ = policy_.IsDenied();

	// Entering test scope
	in_test_scope_ = true;

	// Reset policy if previously violated in previous test
	violated_ = false;
}

void gtest_policies::listener::PolicyListener::StopAndEvaluate()
{
	if (monitor_->Stop())
		ReportViolation();
}

void gtest_policies::listener::PolicyListener::OnTestEnd(
	const ::testing::TestInfo& /*test_info*/ )
{
	if (Policy().IsDenied())
		StopAndEvaluate();

	// Only report policy violations if the test has not failed 
	// due to assertion failure
	const bool has_failure = ::testing::Test::HasFailure();
	if (!has_failure && in_test_scope_ && violated_)
	{
		OnPolicyViolation();

		//GTEST_NONFATAL_FAILURE_(\
		//	"Policy violation: gtest_policies::dynamic_memory_allocation\n" \
		//	"Dynamic memory allocation is not permitted by the test policy " \
		//	"for this test case. " \
		//	"Re-run the test case in debug mode with debugger attached to " \
		//	"break at the allocation causing this policy violation. ");
	}

	// No longer in test scope
	in_test_scope_ = false;

	// No longer applied
	applied_ = false;
	
	// Restore policy setting from before invoking SetUp or test function
	RestorePolicy(stored_policy_);
}

void gtest_policies::listener::PolicyListener::OnTestSuiteEnd(
	const ::testing::TestSuite& /*test_suite*/)
{
	RestorePolicy(program_policy_);
}

void gtest_policies::listener::PolicyListener::OnTestProgramEnd(
	const ::testing::UnitTest& /*unit_test*/)
{ 
	RestorePolicy(global_policy_);
}

void gtest_policies::listener::PolicyListener::RestorePolicy(bool deny) noexcept
{
	if (deny)
		policy_.Deny();
	else
		policy_.Grant();
}

void gtest_policies::listener::PolicyListener::ReportViolation()
{
	if (in_test_scope_ && policy_.IsDenied() && applied_)
		violated_ = true;
}

bool gtest_policies::listener::PolicyListener::IsViolated() const noexcept
{
	return violated_;
}

const gtest_policies::PolicyContext& gtest_policies::listener::PolicyListener::Policy() const noexcept
{
	return policy_;
}

gtest_policies::PolicyContext& gtest_policies::listener::PolicyListener::Policy() noexcept
{
	return policy_;
}

void gtest_policies::listener::PolicyListener::OnPolicyChangeDuringTest(bool deny) noexcept
{
	if (!applied_)
		return; // not applied

	if (deny)
		monitor_->Start(); // grant ---> deny
	else
		StopAndEvaluate(); // deny ---> grant
}

