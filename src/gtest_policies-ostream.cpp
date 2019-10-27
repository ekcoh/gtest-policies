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
