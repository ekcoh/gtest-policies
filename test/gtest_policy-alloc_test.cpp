#include <gtest/gtest.h>
#include <gtest/gtest-spi.h> // enables testing test failures

#include <gtest_policy/gtest_policy.h>

#include <memory>

#include "gtest_policy-policy_test.h"

using namespace gtest_policy;

// Instantiate common test for a policy
INSTANTIATE_TYPED_TEST_SUITE_P(PolicyTest, \
	PolicyTest, DynamicMemoryAllocationPolicyListener);

class DynamicMemoryAllocationPolicyTest2 : 
	public PolicyTest<DynamicMemoryAllocationPolicyListener> { };

TEST_F(DynamicMemoryAllocationPolicyTest2,
	should_fail_test__if_denied_and_allocating_memory_via_global_new)
{
	GivenPreTestSequence();
	policy.Deny();
	std::make_unique<int>(0);
	AssertPostTestSequence(true);
}

TEST_F(DynamicMemoryAllocationPolicyTest2,
	should_not_fail_test__if_granted_and_allocating_memory_via_global_new)
{
	GivenPreTestSequence();
	policy.Grant();
	std::make_unique<int>(0);
	AssertPostTestSequence(false);
}

TEST_F(DynamicMemoryAllocationPolicyTest2,
	should_fail_test__if_denied_and_allocating_memory_via_global_new_array)
{
	GivenPreTestSequence();
	policy.Deny();
	std::unique_ptr<int[]>(new int[5]);
	AssertPostTestSequence(true);
}

TEST_F(DynamicMemoryAllocationPolicyTest2,
	should_not_fail_test__if_granted_and_allocating_memory_via_global_new_array)
{
	GivenPreTestSequence();
	policy.Grant();
	std::unique_ptr<int[]>(new int[5]);
	AssertPostTestSequence(false);
}

TEST_F(DynamicMemoryAllocationPolicyTest2,
	should_fail_test__if_denied_and_allocating_memory_via_malloc)
{
	GivenPreTestSequence();
	policy.Deny();
	auto p = malloc(sizeof(int));
	AssertPostTestSequence(true);

	free(p); // redemtion for leak
}

TEST_F(DynamicMemoryAllocationPolicyTest2,
	should_not_fail_test__if_granted_and_allocating_memory_via_malloc)
{
	GivenPreTestSequence();
	policy.Grant();
	auto p = malloc(sizeof(int));
	AssertPostTestSequence(false);

	free(p); // redemtion for leak
}
