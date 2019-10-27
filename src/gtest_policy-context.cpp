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

gtest_policy::PolicyContext::PolicyContext(
	PolicyListener* listener, bool is_denied_by_default) noexcept : 
	listener_(listener), 
	denied_(is_denied_by_default), 
	denied_by_default_(is_denied_by_default)
{ }

void gtest_policy::PolicyContext::Deny() noexcept
{
	SetDenied(true);
}

void gtest_policy::PolicyContext::Grant() noexcept
{
	SetDenied(false);
}

void gtest_policy::PolicyContext::Apply() noexcept
{
	const auto listener_ptr = listener_;
	if (listener_ptr != nullptr)
		listener_ptr->Apply();
}

void gtest_policy::PolicyContext::SetDenied(bool denied) noexcept
{
	const auto listener_ptr = listener_;
	if (listener_ptr != nullptr && listener_->in_test_scope_)
	{
		const auto previously_denied = denied_;
		denied_ = denied;
		if (listener_ptr->in_test_scope_ && previously_denied != denied)
			listener_ptr->OnPolicyChangeDuringTest(denied);
	}
	else
	{
		denied_ = denied;
	}
}

void gtest_policy::PolicyContext::Reset() noexcept
{
	denied_ = denied_by_default_;
}

bool gtest_policy::PolicyContext::IsDenied() const noexcept
{
	return denied_;
}

bool gtest_policy::PolicyContext::IsViolated() const noexcept
{
	auto ptr = listener_;
	if (ptr != nullptr)
		return listener_->violated_;
	return false;
}

void gtest_policy::PolicyContext::MarkAsViolated() noexcept
{
	auto ptr = listener_;
	if (ptr != nullptr)
		listener_->ReportViolation();
}
