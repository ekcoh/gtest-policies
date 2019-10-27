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