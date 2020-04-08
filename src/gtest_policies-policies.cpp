// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include "gtest_policies/gtest_policies.h"

gtest_policies::PolicyContext
	gtest_policies::dynamic_memory_allocation = gtest_policies::PolicyContext();
gtest_policies::PolicyContext
	gtest_policies::standard_output = gtest_policies::PolicyContext();
gtest_policies::PolicyContext
	gtest_policies::standard_error = gtest_policies::PolicyContext();

void gtest_policies::Apply() noexcept
{
	gtest_policies::dynamic_memory_allocation.Apply();
	gtest_policies::standard_output.Apply();
	gtest_policies::standard_error.Apply();
}

//gtest_policies::PolicyContext gtest_policies::xxx = gtest_policies::PolicyContext();
//gtest_policies::PolicyContext gtest_policies::xxx = gtest_policies::PolicyContext();