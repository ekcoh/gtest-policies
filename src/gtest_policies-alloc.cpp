// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#ifndef GTEST_POLICY_ALLOC_H
#define GTEST_POLICY_ALLOC_H

#include <gtest_policies/gtest_policies.h>

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

namespace gtest_policies
{
#ifdef GTEST_POLICY_CRTDBG_AVAILABLE
	_CRT_ALLOC_HOOK stored_alloc_hook = nullptr;
#endif

	class AllocMonitor : public gtest_policies::detail::PolicyMonitor
	{
	public:
#ifdef GTEST_POLICY_CRTDBG_AVAILABLE
		AllocMonitor()
			: pre_count_(0u)
		{ 
		}
#else
		AllocMonitor() { }
#endif

		~AllocMonitor() = default;

		void Start() 
		{
#ifdef GTEST_POLICY_CRTDBG_AVAILABLE
			if (IsDebuggerPresent())
			{
				stored_alloc_hook = _CrtSetAllocHook(AllocHook);
			}

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
			{
				_CrtSetAllocHook(stored_alloc_hook);
				stored_alloc_hook = nullptr;
			}

			const auto diff = post_count - pre_count_;
			return diff != 0; // memory was allocated or not
#endif // GTEST_POLICY_CRTDBG_AVAILABLE
            return false;
		}

	private:
#ifdef GTEST_POLICY_CRTDBG_AVAILABLE
		static inline int __cdecl InvokeWrappedAllocHook(
			int nAllocType,
			void* pvData,
			size_t nSize,
			int nBlockUse,
			long lRequest,
			const unsigned char * szFileName,
			int nLine)
		{
			if (stored_alloc_hook == nullptr)
				return TRUE;
			return stored_alloc_hook(nAllocType, pvData, nSize, 
				nBlockUse, lRequest, szFileName, nLine);
		}

		static int __cdecl AllocHook(
			int nAllocType, 
			void* pvData,
			size_t nSize, 
			int nBlockUse,
			long lRequest,
			const unsigned char * szFileName,
			int nLine)
		{
			if (nAllocType == _HOOK_FREE)
				return InvokeWrappedAllocHook(nAllocType, pvData, nSize, nBlockUse, lRequest, szFileName, nLine);

			// IMPORTANT INFORMATION:
			// If your debugger breaks here it means allocation has been denied 
			// explicitly by the gtest_policies::dynamic_memory_allocation policy. 
			// Check your current call stack to find out where this memory allocation 
			// originates from.
			if (gtest_policies::dynamic_memory_allocation.IsDenied() &&
				gtest_policies::dynamic_memory_allocation.IsViolated() &&
				IsDebuggerPresent())
			{
				DebugBreak();
			}

			return InvokeWrappedAllocHook(nAllocType, pvData, nSize, nBlockUse, lRequest, szFileName, nLine);
		}

		size_t pre_count_;
#endif
	};
}

#ifdef GTEST_POLICY_CRTDBG_AVAILABLE

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
	gtest_policies::policies::dynamic_memory_allocation.MarkAsViolated();
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
	gtest_policies::policies::dynamic_memory_allocation.MarkAsViolated();
	return malloc(s);
}

void operator delete[](void *p) throw()
{
	free(p);
}

#endif // GTEST_POLICY_CRTDBG_AVAILABLE

gtest_policies::listener::MemAllocPolicyListener::MemAllocPolicyListener() :
	PolicyListener(dynamic_memory_allocation, std::make_unique<AllocMonitor>())
{ }

void gtest_policies::listener::MemAllocPolicyListener::OnPolicyViolation()
{
	GTEST_NONFATAL_FAILURE_(\
		"Policy violation: gtest_policy::dynamic_memory_allocation\n" \
		"Dynamic memory allocation is not permitted by the test policy " \
		"for this test case. " \
		"Re-run the test case in debug mode with debugger attached to " \
		"break at the allocation causing this policy violation. ");
}

#endif // GTEST_POLICY_ALLOC_H