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
