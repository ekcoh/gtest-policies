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

#include "gtest_policies-policy_test.h"

using namespace gtest_policies;
using namespace gtest_policies::listener;

// Instantiate common test for a policy
INSTANTIATE_TYPED_TEST_SUITE_P(AllocPolicyTest, \
	PolicyTest, MemAllocPolicyListener);

class DynamicMemoryAllocationPolicyTest : 
	public PolicyTest<MemAllocPolicyListener> { };

TEST_F(DynamicMemoryAllocationPolicyTest,
	should_fail_test__if_denied_and_allocating_memory_via_global_new)
{
	GivenPreTestSequence();
	policy.Deny();
	std::make_unique<int>(0);
	AssertPostTestSequence(true);
}

TEST_F(DynamicMemoryAllocationPolicyTest,
	should_not_fail_test__if_granted_and_allocating_memory_via_global_new)
{
	GivenPreTestSequence();
	policy.Grant();
	std::make_unique<int>(0);
	AssertPostTestSequence(false);
}

TEST_F(DynamicMemoryAllocationPolicyTest,
	should_fail_test__if_denied_and_allocating_memory_via_global_new_array)
{
	GivenPreTestSequence();
	policy.Deny();
	std::unique_ptr<int[]>(new int[5]);
	AssertPostTestSequence(true);
}

TEST_F(DynamicMemoryAllocationPolicyTest,
	should_not_fail_test__if_granted_and_allocating_memory_via_global_new_array)
{
	GivenPreTestSequence();
	policy.Grant();
	std::unique_ptr<int[]>(new int[5]);
	AssertPostTestSequence(false);
}

TEST_F(DynamicMemoryAllocationPolicyTest,
	should_fail_test__if_denied_and_allocating_memory_via_malloc)
{
	GivenPreTestSequence();
	policy.Deny();
	auto p = malloc(sizeof(int));
	AssertPostTestSequence(true);

	free(p); // redemtion for leak
}

TEST_F(DynamicMemoryAllocationPolicyTest,
	should_not_fail_test__if_granted_and_allocating_memory_via_malloc)
{
	GivenPreTestSequence();
	policy.Grant();
	auto p = malloc(sizeof(int));
	AssertPostTestSequence(false);

	free(p); // redemtion for leak
}
