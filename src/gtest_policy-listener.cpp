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

#include <gtest_policy/gtest_policy.h>

gtest_policy::PolicyListener::PolicyListener(PolicyContext& policy, std::unique_ptr<PolicyMonitor>&& monitor) noexcept : 
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

gtest_policy::PolicyListener::~PolicyListener() noexcept
{
	policy_.listener_ = nullptr;
}

void gtest_policy::PolicyListener::OnTestProgramStart(
	const ::testing::UnitTest& /*unit_test*/)
{ 
	policy_.listener_ = this;
	global_policy_ = policy_.IsDenied();
}

void gtest_policy::PolicyListener::OnTestSuiteStart(
	const ::testing::TestSuite& /*test_suite*/)
{
	program_policy_ = policy_.IsDenied();
}

void gtest_policy::PolicyListener::Apply()
{
	if (!applied_)
	{
		applied_ = true;
		if (Policy().IsDenied())
			monitor_->Start();
	}
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

void gtest_policy::PolicyListener::StopAndEvaluate()
{
	if (monitor_->Stop())
		ReportViolation();
}

void gtest_policy::PolicyListener::OnTestEnd(
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
		//	"Policy violation: gtest_policy::dynamic_memory_allocation\n" \
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

void gtest_policy::PolicyListener::OnTestSuiteEnd(
	const ::testing::TestSuite& /*test_suite*/)
{
	RestorePolicy(program_policy_);
}

void gtest_policy::PolicyListener::OnTestProgramEnd(
	const ::testing::UnitTest& /*unit_test*/)
{ 
	RestorePolicy(global_policy_);
}

void gtest_policy::PolicyListener::RestorePolicy(bool deny) noexcept
{
	if (deny)
		policy_.Deny();
	else
		policy_.Grant();
}

void gtest_policy::PolicyListener::ReportViolation()
{
	if (in_test_scope_ && policy_.IsDenied() && applied_)
		violated_ = true;
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

void gtest_policy::PolicyListener::OnPolicyChangeDuringTest(bool deny) noexcept
{
	if (!applied_)
		return; // not applied

	if (deny)
		monitor_->Start(); // grant ---> deny
	else
		StopAndEvaluate(); // deny ---> grant
}

