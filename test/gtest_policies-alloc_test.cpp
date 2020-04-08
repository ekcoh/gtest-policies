// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

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
