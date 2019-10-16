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

gtest_policy::PolicyContext
gtest_policy::policies::dynamic_memory_allocation = gtest_policy::PolicyContext();

namespace gtest_policy
{
	class DynamicMemoryAllocationPolicyListener::Impl
	{
	public:
#ifdef GTEST_POLICY_CRTDBG_AVAILABLE
		Impl(PolicyListener* listener) 

			: listener_(listener), 
			pre_count_(0u) 
		{ }
#else
		Impl(PolicyListener*) { }
#endif

		~Impl() = default;

		void OnTestStart() 
		{
#ifdef GTEST_POLICY_CRTDBG_AVAILABLE
			_CrtMemState state;
			_CrtMemCheckpoint(&state);
			pre_count_ = state.lTotalCount;

			if (IsDebuggerPresent())
				_CrtSetAllocHook(AllocHook);
#endif // GTEST_POLICY_CRTDBG_AVAILABLE
		}

		void OnTestEnd() 
		{
#ifdef GTEST_POLICY_CRTDBG_AVAILABLE
			if (IsDebuggerPresent())
				_CrtSetAllocHook(nullptr);

			_CrtMemState state;
			_CrtMemCheckpoint(&state);

			if (state.lTotalCount - pre_count_ != 0)
				listener_->ReportViolation();

#endif // GTEST_POLICY_CRTDBG_AVAILABLE
		}

		void OnPolicyViolation()
		{
			GTEST_NONFATAL_FAILURE_(\
				"Policy violation: gtest_policy::dynamic_memory_allocation\n" \
				"Dynamic memory allocation is not permitted by the test policy " \
				"for this test case. " \
				"Re-run the test case in debug mode with debugger attached to " \
				"break at the allocation causing this policy violation. ");
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

		PolicyListener* listener_;
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
	PolicyListener(policies::dynamic_memory_allocation), 
	impl_(std::make_unique<DynamicMemoryAllocationPolicyListener::Impl>(this))
{ }

gtest_policy::DynamicMemoryAllocationPolicyListener::~DynamicMemoryAllocationPolicyListener()
{ } // empty, required for PIMPL

void gtest_policy::DynamicMemoryAllocationPolicyListener::OnTestStart(
	const ::testing::TestInfo& test_info) 
{
	PolicyListener::OnTestStart(test_info);
	if (Policy().IsDenied())
		impl_->OnTestStart();
}

void gtest_policy::DynamicMemoryAllocationPolicyListener::OnTestEnd(
	const ::testing::TestInfo& test_info) 
{
	if (Policy().IsDenied())
		impl_->OnTestEnd();
	PolicyListener::OnTestEnd(test_info);
}

void gtest_policy::DynamicMemoryAllocationPolicyListener::OnPolicyViolation()
{
	impl_->OnPolicyViolation();
}

void gtest_policy::DynamicMemoryAllocationPolicyListener::OnPolicyChangeDuringTest(bool deny) noexcept
{
	if (deny) // Transition from Grant -> deny
		impl_->OnTestStart();
	else // Transition from deny -> Grant      
		impl_->OnTestEnd();	
}

#endif // GTEST_POLICY_ALLOC_H