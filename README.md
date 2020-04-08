[![Build status](https://ci.appveyor.com/api/projects/status/gof9v7avc6jmgt9k?svg=true)](https://ci.appveyor.com/project/ekcoh/gtest-policies)

# gtest_policies
Google test policy extensions for even better C++ unit testing by introducing code policies that helps detecting undesired constructs in own or submodule code. The project supports C++11 and above. Some features are limited in functionality on certain platforms and configurations, but full feature set is available on Windows with MSVC tool-chain.

## Features
- Support for dynamically denying and granting policies on program, test suite or test level.
- Support for automatic policy revert when exiting current test scope.
- Violated policies are reported as Google Test failures and hence are visible in CI.
- Policies:
	- Dynamic memory allocation policy (gtest_policies::dynamic_memory_allocation)
		- Detect and fail tests if implementation allocate dynamic memory.
		- Quickly find undesired allocations via stack trace and dynamic debug break points when debugging.
		- Useful to detect unwanted dynamic memory allocations from own or third party code compiled with the project, for example unwanted or unexpected dynamic memory allocations from STL implementation.
	- Standard output policy (gtest_policies::standard_output)
		- Detect and fail tests if implementation writes to std::cout.
		- Quickly find undesired writes to std::cout via stack trace and dynamic debug break points when debugging.
		- Useful to detect typical "mistakes" where developers have added debug print outs to code base.
	- Standard error policy (gtest_policies::standard_error)
		- Detect and fail tests if implementation writes to std::cerr.
		- Quickly find undesired writes to std::cerr via stack trace and dynamic debug break points when debugging.
		- Useful to detect typical "mistakes" where developers have added debug print outs to code base.		

## Requirements
The project depends on the open source [Google Test](https://github.com/google/googletest) project. You can import and add Google Test yourself or let the CMake script of this project download and build it for you by setting the CMake property GTEST_POLICIES_DOWNLOAD_GTEST to ON (default). If you already have Google Test added to your project, set GTEST_POLICIES_DOWNLOAD_GTEST to OFF.

## Getting started
To start using the policy extensions, simply add the project as a sub-directory to your existing CMake project. Then, include the gtest_policies header:

```cpp
#include <gtest_policies/gtest_policies.h>  // Include this policy extension
```

In the main function of the test program, directly after initializing Google Test, add policy support by adding the policy listeners of interest, e.g:

```cpp
int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	::testing::UnitTest::GetInstance()->listeners().Append(
		new gtest_policy::listener::MemoryAllocationPolicyListener());
	::testing::UnitTest::GetInstance()->listeners().Append(
		new gtest_policy::listener::StandardOutputStreamPolicyListener());
	::testing::UnitTest::GetInstance()->listeners().Append(
		new gtest_policy::listener::StandardErrorStreamPolicyListener());		
	return RUN_ALL_TESTS();
}
```
...or use one of the convenience macros:

```cpp
int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	GTEST_POLICIES_APPEND_ALL_LISTENERS;
	return RUN_ALL_TESTS();
}
```

...or even shorter:

```cpp
GTEST_POLICIES_MAIN
```

All policies are denied by default. However, in order to make them active it is required to apply them from either the test itself or for a whole test suite by applying it in SetUp() method (recommended). The reason this is required is related to how Google Test is currently implemented. 

The approach that requires the least amount of work is to apply all policies (only affects policies added from main) in the SetUp() method of the test fixture:

```cpp
class MyFixture : public ::testing::Test 
{ 
   void SetUp() override
   {
      gtest_policies::Apply();
   }
};

TEST_F(MyFixture, MyTest)
{
   // Test implementation...
}
```

...or derive from a test class doing it for you already:

```cpp
class fixture : public gtest_policies::Test { };

TEST_F(MyFixture, MyTest)
{
   // Test implementation...
}
```

A complete example of the basic setup can be found in [example/01_getting_started](example/01_getting_started)
More examples can be found in [example/](example) folder.

## Dynamic Memory Allocation Policy

The gtest_policies::MemAllocPolicyListener manages the following policies:
- gtest_policies::dynamic_memory_allocation

The detection of dynamic memory allocation relies on the [CRT Heap Debug](https://docs.microsoft.com/en-us/visualstudio/debugger/crt-debug-heap-details?view=vs-2019) API provided by Microsoft and hence it is only supported for C++ projects built with Microsoft Visual Studio build tools. It also means that policy violations may only be detected when running debug test builds.

In order to use the MemoryPolicyListener it must be added as a test event listener before running the tests, e.g.

```cpp
::testing::UnitTest::GetInstance()->listeners().Append(
	new gtest_policies::listener::MemAllocPolicyListener());
```

If this policy is denied any detected dynamic memory allocation via new, malloc etc. will be reported as a failed unit test. In order to detect where allocation occurrs, re-run failed tests in debug mode to break at the allocation and follow the stack trace to find the allocation call.

## Standard Output Allocation Policy

The gtest_policies::StdOutPolicyListener manages the following policies:
- gtest_policies::standard_output

The detection of standard output writes relies on substituting the default std::cout instance with a filter that forwards data but detects any writes. This makes it possible to identify and report this as a policy violation.

## Standard Error Allocation Policy

The gtest_policies::StdErrPolicyListener manages the following policies:
- gtest_policies::standard_error

The detection of standard error writes relies on substituting the default std::cerr instance with a filter that forwards data but detects any writes. This makes it possible to identify and report this as a policy violation.

## Known Limitations
- It would be convenient to not have to call gtest_policies::Apply() in the SetUp method of all tests. However, due to limitations and implementation specific details of Google Test this is currently not possible. This can easily be managed though by explicitly denying them in the SetUp method of the fixture, possibly in a shared base class like gtest_policies::policy_test. This might change in the future if Google Test implement callbacks around the test implementation run method.
- Dynamic memory allocation policy violations is currently only supported in MSVC via CRT Heap Debug builds in debug mode. On other configurations or tool-chains this policy reverts to basic global overloading of new and delete operators.

## License

This project is released under the MIT license, see [License](https://github.com/ekcoh/gtest-policies/blob/master/LICENSE).
This basically means you can do whatever you want with this project as long as you provide the original license and copyright information with your own project distribution.
