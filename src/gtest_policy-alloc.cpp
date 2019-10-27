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

#ifndef GTEST_POLICY_ALLOC_H
#define GTEST_POLICY_ALLOC_H

#include <gtest_policy/gtest_policy.h>

#ifdef _MSC_VER
  #ifdef _DEBUG
    #ifndef GTEST_POLICY_CRTDBG_AVAILABLE
      #define GTEST_POLICY_CRTDBG_AVAILABLE
    #endif // GTEST_POLICY_CRTDBG_AVAILABLE
    
    // CRT debug tools
    #ifndef _CRTDBG_MAP_ALLOC
      #define _CRTDBG_MAP_ALLOC
    #endif // _CRTDBG_MAP_ALLOC

    #include <crtdbg.h>
  #else // _DEBUG
    #ifndef GTEST_POLICY_SILENCE_WARNINGS
      #pragma message ( \
        "WARNING: gtest_policy::dynamic_memory_allocation policy." \
	    "Memory allocation detection not supported for release build." \
        "Detection only possible via globally overriding new/delete.")
    #endif // GTEST_POLICY_SILENCE_WARNINGS
  #endif // _DEBUG
#else
  #ifndef GTEST_POLICY_SILENCE_WARNINGS
    #pragma message ( \
      "WARNING: gtest_policy::dynamic_memory_allocation policy." \
	  "Memory allocation detection not supported on this compiler/platform." \
      "Detection only possible via globally overriding new/delete.");
  #endif // GTEST_POLICY_SILENCE_WARNINGS
#endif // _MSC_VER

#ifdef GTEST_POLICY_CRTDBG_AVAILABLE
  #if defined(min) || defined(max)
    #error Windows.h needs to be included by this header, or else NOMINMAX needs \
           to be defined before including it yourself.
  #endif // defined(min) || defined(max)

  // Avoid inteference with std::numeric_limits
  #ifndef NOMINMAX
    #define NOMINMAX 1
  #endif // NOMINMAX

  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN 
  #endif // WIN32_LEAN_AND_MEAN

  #include <Windows.h> // IsDebuggerPresent, DebugBreak
#endif // GTEST_POLICY_CRTDBG_AVAILABLE

namespace gtest_policy
{
	class AllocMonitor : public gtest_policy::PolicyMonitor
	{
	public:
#ifdef GTEST_POLICY_CRTDBG_AVAILABLE
		AllocMonitor()
			: pre_count_(0u) 
		{ }
#else
		AllocMonitor(PolicyListener*) { }
#endif

		~AllocMonitor() = default;

		void Start() 
		{
#ifdef GTEST_POLICY_CRTDBG_AVAILABLE
			if (IsDebuggerPresent())
				_CrtSetAllocHook(AllocHook);

			_CrtMemState state;
			_CrtMemCheckpoint(&state);
			pre_count_ = state.lTotalCount;
#endif // GTEST_POLICY_CRTDBG_AVAILABLE
		}

		bool Stop() 
		{
#ifdef GTEST_POLICY_CRTDBG_AVAILABLE
			_CrtMemState state;
			_CrtMemCheckpoint(&state);
			const auto post_count = state.lTotalCount;

			if (IsDebuggerPresent())
				_CrtSetAllocHook(nullptr);

			const auto diff = post_count - pre_count_;
			return diff != 0;
#endif // GTEST_POLICY_CRTDBG_AVAILABLE
		}

	private:
#ifdef GTEST_POLICY_CRTDBG_AVAILABLE
		static int __cdecl AllocHook(
			int nAllocType, 
			void* /*pvData*/,
			size_t /*nSize*/, 
			int /*nBlockUse*/,
			long /*lRequest*/,
			const unsigned char * /*szFileName*/,
			int /*nLine*/)
		{
			if (nAllocType == _HOOK_FREE)
				return TRUE;

			// IMPORTANT INFORMATION:
			// If your debugger breaks here it means allocation has been denied 
			// explicitly by the gtest_policy::dynamic_memory_allocation policy. 
			// Check your current call stack to find out where this memory allocation 
			// originates from.
			
			// TODO In test scope && not violated
			if (gtest_policy::policies::dynamic_memory_allocation.IsDenied() &&
				gtest_policy::policies::dynamic_memory_allocation.IsViolated() &&
				IsDebuggerPresent())
			{
				DebugBreak();
			}

			return TRUE; // Allow the memory operation to proceed
		}

		size_t pre_count_;
#endif
	};
}

#ifndef GTEST_POLICY_CRTDBG_AVAILABLE

// Override global new/delete and use malloc/free as underlying functions.
// Makes it possible to intercept all allocation being done with regular 
// new/delete operators.

#ifdef _MSC_VER
// MSVC C++11/14/17 do not support throw(exception) 
void* operator new(std::size_t s) throw()
#else
void* operator new(std::size_t s) throw(std::bad_alloc)
#endif
{
	gtest_policy::policies::dynamic_memory_allocation.MarkAsViolated();
	return malloc(s);
}

void  operator delete(void* p) throw()
{
	free(p);
}

#ifdef _MSC_VER
// MSVC C++11/14/17 do not support throw(exception) 
void *operator new[](std::size_t s) throw()
#else
void *operator new[](std::size_t s) throw(std::bad_alloc)
#endif
{
	gtest_policy::policies::dynamic_memory_allocation.MarkAsViolated();
	return malloc(s);
}

void operator delete[](void *p) throw()
{
	free(p);
}

#endif

gtest_policy::DynamicMemoryAllocationPolicyListener::DynamicMemoryAllocationPolicyListener() : 
	PolicyListener(policies::dynamic_memory_allocation, std::make_unique<AllocMonitor>())
{ }

void gtest_policy::DynamicMemoryAllocationPolicyListener::OnPolicyViolation()
{
	GTEST_NONFATAL_FAILURE_(\
		"Policy violation: gtest_policy::dynamic_memory_allocation\n" \
		"Dynamic memory allocation is not permitted by the test policy " \
		"for this test case. " \
		"Re-run the test case in debug mode with debugger attached to " \
		"break at the allocation causing this policy violation. ");
}

#endif // GTEST_POLICY_ALLOC_H