// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include <gtest_policies/gtest_policies.h>

gtest_policies::PolicyContext::PolicyContext(
	listener::PolicyListener* listener, bool is_denied_by_default) noexcept : 
	listener_(listener), 
	denied_(is_denied_by_default), 
	denied_by_default_(is_denied_by_default)
{ }

void gtest_policies::PolicyContext::Deny() noexcept
{
	SetDenied(true);
}

void gtest_policies::PolicyContext::Grant() noexcept
{
	SetDenied(false);
}

void gtest_policies::PolicyContext::Apply() noexcept
{
	const auto listener_ptr = listener_;
	if (listener_ptr != nullptr)
		listener_ptr->Apply();
}

void gtest_policies::PolicyContext::SetDenied(bool denied) noexcept
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

void gtest_policies::PolicyContext::Reset() noexcept
{
	denied_ = denied_by_default_;
}

bool gtest_policies::PolicyContext::IsDenied() const noexcept
{
	return denied_;
}

bool gtest_policies::PolicyContext::IsViolated() const noexcept
{
	auto ptr = listener_;
	if (ptr != nullptr)
		return listener_->violated_;
	return false;
}

void gtest_policies::PolicyContext::MarkAsViolated() noexcept
{
	auto ptr = listener_;
	if (ptr != nullptr)
		listener_->ReportViolation();
}
