// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include <gtest_policies/gtest_policies.h>

#include <iostream>
#include <cctype>
#include <cstdio>

namespace gtest_policies
{
	class CountingStreamBufferFilter : public std::streambuf
	{
	public:
		CountingStreamBufferFilter(std::streambuf* dst)
			: cnt_(0u), dst_(dst)
		{ 
			assert(dst);
		}

		size_t count()
		{
			return cnt_;
		}

		void reset()
		{
			cnt_ = 0;
		}

	protected:
		virtual int_type overflow(int_type c) 
		{
			++cnt_;

			int_type result(EOF);
			if (c == EOF)
				result = sync();
			else if (dst_)
			{
				assert(c >= 0 && c <= UCHAR_MAX);
				result = dst_->sputc(static_cast<char>(c));
			}
			return result;
		}

	private:
		size_t cnt_;
		std::streambuf* dst_;
	};

	template<class Char, class Traits = std::char_traits<Char>>
	class OutputStreamMonitor : public gtest_policies::detail::PolicyMonitor
	{
	public:
		OutputStreamMonitor(std::basic_ostream<Char, Traits>& ostream) 
			: stream_(ostream), original_(ostream.rdbuf()), filter_(ostream.rdbuf())
		{
			ostream.rdbuf(&filter_);
		}

		~OutputStreamMonitor()
		{
			stream_.rdbuf(original_);
		}

		void Start() override
		{
			filter_.reset();
		}

		bool Stop() override
		{
			return filter_.count() > 0u;
		}

		std::basic_ostream<Char, Traits>& stream_;
		std::streambuf* original_;
		CountingStreamBufferFilter filter_;
	};
}

gtest_policies::listener::StdOutPolicyListener::StdOutPolicyListener()
	: PolicyListener(standard_output, std::make_unique<OutputStreamMonitor<char>>(std::cout))
{ }

void gtest_policies::listener::StdOutPolicyListener::OnPolicyViolation()
{
	GTEST_NONFATAL_FAILURE_(\
		"Policy violation: gtest_policy::cxx_std_out\n" \
		"Writing to standard output is not permitted by the test policy " \
		"for this test case. " \
		"Re-run the test case in debug mode with debugger attached to " \
		"break at the statement causing this policy violation. ");
}

gtest_policies::listener::StdErrPolicyListener::StdErrPolicyListener()
	: PolicyListener(standard_error, std::make_unique<OutputStreamMonitor<char>>(std::cerr))
{ }

void gtest_policies::listener::StdErrPolicyListener::OnPolicyViolation()
{
	GTEST_NONFATAL_FAILURE_(\
		"Policy violation: gtest_policy::cxx_std_err\n" \
		"Writing to standard error is not permitted by the test policy " \
		"for this test case. " \
		"Re-run the test case in debug mode with debugger attached to " \
		"break at the statement causing this policy violation. ");
}
