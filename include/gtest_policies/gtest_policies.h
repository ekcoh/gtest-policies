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

#ifndef GTEST_POLICIES_H
#define GTEST_POLICIES_H

#include <gtest/gtest.h> // Google Test
#include <memory>        // std::unique_ptr

#ifndef GTEST_POLICIES_APPEND_ALL_LISTENERS
#define GTEST_POLICIES_APPEND_ALL_LISTENERS \
	::testing::UnitTest::GetInstance()->listeners().Append( \
		new gtest_policies::listener::MemAllocPolicyListener()); \
	::testing::UnitTest::GetInstance()->listeners().Append( \
		new gtest_policies::listener::StdOutPolicyListener()); \
	::testing::UnitTest::GetInstance()->listeners().Append( \
		new gtest_policies::listener::StdErrPolicyListener())
#endif // GTEST_POLICIES_APPEND_ALL_LISTENERS

// Convenience macro to generate a main program entry point with policy 
// extension enabled by adding test listeners. Listeners are freed by 
// the framework on shutdown.
#ifndef GTEST_POLICIES_MAIN
#define GTEST_POLICIES_MAIN \
int main(int argc, char **argv) \
{ \
	::testing::InitGoogleTest(&argc, argv); \
	GTEST_POLICIES_APPEND_ALL_LISTENERS; \
	return RUN_ALL_TESTS(); \
}
#endif // #ifndef GTEST_POLICY_MAIN

namespace gtest_policies {

class PolicyContext;

namespace listener {
 class PolicyListener;
}

///////////////////////////////////////////////////////////////////////////////
// PolicyContext
///////////////////////////////////////////////////////////////////////////////

class PolicyContext {
 public:
  explicit PolicyContext(listener::PolicyListener* listener = nullptr,
	  bool is_denied_by_default = true) noexcept;
  PolicyContext(const PolicyContext&) noexcept = default;
  ~PolicyContext() noexcept = default;

  void Deny() noexcept;
  void Grant() noexcept;
  void Apply() noexcept;

  void SetDenied(bool denied) noexcept;

  void Reset() noexcept;

  bool IsDenied() const noexcept;
  bool IsViolated() const noexcept;
  void MarkAsViolated() noexcept; // INTERNAL - DO NOT CALL
 
private:
  friend gtest_policies::listener::PolicyListener;

  listener::PolicyListener* listener_;
  bool denied_;
  bool denied_by_default_;
};

///////////////////////////////////////////////////////////////////////////////
// Test policies
///////////////////////////////////////////////////////////////////////////////

extern PolicyContext dynamic_memory_allocation;
extern PolicyContext standard_output;
extern PolicyContext standard_error;

void Apply() noexcept;

///////////////////////////////////////////////////////////////////////////////
// Test
///////////////////////////////////////////////////////////////////////////////

class Test : public ::testing::Test
{
public:
	virtual ~Test() = default;
protected:
	Test() = default;

	virtual void SetUp() override
	{
		::testing::Test::SetUp();
		gtest_policies::Apply();
	}

	virtual void TearDown() override
	{
		::testing::Test::TearDown();
	}
};

///////////////////////////////////////////////////////////////////////////////
// PolicyListener
///////////////////////////////////////////////////////////////////////////////

namespace detail {

class PolicyMonitor
{
public:
	virtual ~PolicyMonitor() { }
	virtual void Start() = 0;
	virtual bool Stop() = 0;
};

} // namespace gtest_policies::detail

///////////////////////////////////////////////////////////////////////////////
// PolicyListener
///////////////////////////////////////////////////////////////////////////////

namespace listener {

class PolicyListener : public ::testing::TestEventListener {
public:
	PolicyListener(PolicyContext& policy,
		std::unique_ptr<detail::PolicyMonitor>&& monitor) noexcept;
	virtual ~PolicyListener() noexcept;

	void OnTestProgramStart(
		const ::testing::UnitTest& unit_test) override;
	void OnTestIterationStart(
		const ::testing::UnitTest& /*unit_test*/,
		int /*iteration*/) override { }
	void OnEnvironmentsSetUpStart(
		const ::testing::UnitTest& /*unit_test*/) override { }
	void OnEnvironmentsSetUpEnd(
		const ::testing::UnitTest& /*unit_test*/) override { }
	void OnTestSuiteStart(
		const ::testing::TestSuite& test_suite) override;
#ifndef GTEST_REMOVE_LEGACY_TEST_CASEAPI_
	void OnTestCaseStart(
		const ::testing::TestCase& /*test_case*/) override { }
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI_
	void OnTestStart(
		const ::testing::TestInfo& test_info) override;
	void OnTestPartResult(
		const ::testing::TestPartResult& /*test_part_result*/) override { }
	void OnTestEnd(
		const ::testing::TestInfo& test_info) override;
	void OnTestSuiteEnd(
		const ::testing::TestSuite& test_suite) override;
#ifndef GTEST_REMOVE_LEGACY_TEST_CASEAPI_
	void OnTestCaseEnd(
		const ::testing::TestCase& /*test_case*/) override { }
#endif  //  GTEST_REMOVE_LEGACY_TEST_CASEAPI_
	void OnEnvironmentsTearDownStart(
		const ::testing::UnitTest& /*unit_test*/) override { }
	void OnEnvironmentsTearDownEnd(
		const ::testing::UnitTest& /*unit_test*/) override { }
	void OnTestIterationEnd(
		const ::testing::UnitTest& /*unit_test*/,
		int /*iteration*/) override { }
	void OnTestProgramEnd(
		const ::testing::UnitTest& unit_test) override;

	bool IsViolated() const noexcept;
	const PolicyContext& Policy() const noexcept;
	PolicyContext& Policy() noexcept;

protected:
	//virtual void OnTestPartPolicyViolation() {};
	virtual void OnPolicyViolation() {};

private:
	void Apply();
	void ReportViolation();
	void StopAndEvaluate();
	void RestorePolicy(bool Deny) noexcept;
	void OnPolicyChangeDuringTest(bool Deny) noexcept;

	std::unique_ptr<detail::PolicyMonitor> monitor_;
	PolicyContext& policy_;
	bool global_policy_;
	bool program_policy_;
	bool stored_policy_;
	bool violated_;
	bool in_test_scope_;
	bool applied_;

	friend gtest_policies::PolicyContext;
};

///////////////////////////////////////////////////////////////////////////////
// MemAllocPolicyListener
///////////////////////////////////////////////////////////////////////////////

class MemAllocPolicyListener : public PolicyListener
{
public:
	MemAllocPolicyListener();
protected:
	void OnPolicyViolation() override;
};

///////////////////////////////////////////////////////////////////////////////
// StdOutPolicyListener
///////////////////////////////////////////////////////////////////////////////

class StdOutPolicyListener : public PolicyListener
{
public:
	StdOutPolicyListener();
protected:
	void OnPolicyViolation() override;
};

///////////////////////////////////////////////////////////////////////////////
// StdErrPolicyListener
///////////////////////////////////////////////////////////////////////////////

class StdErrPolicyListener : public PolicyListener
{
public:
	StdErrPolicyListener();
protected:
	void OnPolicyViolation() override;
};

} // namespace gtest_policies::listener

} // namespace gtest_policies

#endif // GTEST_POLICIES_H