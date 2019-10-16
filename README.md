# gtest_policy
Google test policy extensions for even better C++ unit testing by introducing code policies that helps detecting undesired constructs in own or submodule code. The project supports C++11 and above. Some features are limited in functionality on certain platforms and configurations, but full feature set is available on Windows with MSVC tool-chain.

## Features
- Support for dynamically denying and granting policies on program, test suite or test level.
- Support for automatic policy revert when exiting current test scope.
- Violated policies are reported as Google Test failures and hence are visible in CI.
- Policies:
	- Dynamic memory allocation policy.
		- Detect and fail tests if implementation allocate dynamic memory.
		- Quickly find undesired allocations via stack trace and debug break when debugging.
		- Useful to detect unwanted dynamic memory allocations from own or third party code compiled with the project, for example unwanted or unexpected dynamic memory allocations from STL.

## Requirements
The project depends on the open source [Google Test](https://github.com/google/googletest) project. You can import and add Google Test yourself or let the CMake script of this project download and build it for you by setting the CMake property GTEST_POLICY_DOWNLOAD_GTEST to ON (default). If you already have Google Test added to your project, set GTEST_POLICY_DOWNLOAD_GTEST to OFF.

## Getting started
To start using the policy extensions, simply add the project as a sub-directory to your existing CMake project. Then, include the gtest_policy header:

```cpp
#include <gtest_policy/gtest_policy.h>  // Include this policy extension
```

In the main function of the test program, directly after initializing Google Test, add policy support by adding the policy listeners of interest, e.g:

```cpp
int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	::testing::UnitTest::GetInstance()->listeners().Append(
		new gtest_policy::DynamicMemoryAllocationPolicyListener());
	return RUN_ALL_TESTS();
}
```

A complete example is available in [example/01_dynamic_memory_allocation](example/01_dynamic_memory_allocation).

## Dynamic Memory Allocation Policy

The gtest_policy::DynamicMemoryAllocationPolicyListener manages the following policies:
- gtest_policy::policies::dynamic_memory_allocation

The detection of dynamic memory allocation relies on the [CRT Heap Debug](https://docs.microsoft.com/en-us/visualstudio/debugger/crt-debug-heap-details?view=vs-2019) API provided by Microsoft and hence it is only supported for C++ projects built with Microsoft Visual Studio build tools. It also means that policy violations may only be detected when running debug test builds.

In order to use the MemoryPolicyListener it must be added as a test event listener before running the tests, e.g.

```cpp
::testing::UnitTest::GetInstance()->listeners().Append(
	new gtest_policy::DynamicMemoryAllocationPolicyListener());
```

If this policy is denied any detected dynamic memory allocation via new, malloc etc. will be reported as a failed unit test. In order to detect where allocation occurss, re-run failed tests in debug mode to break at the allocation and follow the stack trace to find the allocation call.

## Known Limitations
- Dynamic memory allocation policy violations is currently only supported in MSVC via CRT Heap Debug builds in debug mode. On other configurations or tool-chains this policy reverts to basic global overloading of new and delete operators.

## License

This project is released under the MIT license, see [License](https://github.com/ekcoh/gtest-policies/blob/master/LICENSE).
