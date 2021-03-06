#pragma once

/**********************************************************************
    class: MemoryTracker + GlobalAllocator (memory_tracker.h)

    author: S. Hau
    date: October 7, 2017

    Tracks all allocated memory from global new/delete.
**********************************************************************/

#include "Memory/memory_structs.h"

namespace Core { namespace MemoryManagement {

    //**********************************************************************
    // Global new/delete call this functions in order to allocate/deallocate.
    // Manipulates the AllocationMemoryInfo struct in the MemoryTracker class.
    //**********************************************************************
    class _GlobalNewAndDeleteAllocator
    {
    public:
        static void* allocate(Size size);
        static void  deallocate(void* mem);

        static void* allocateDebug(Size size, const char* file, U32 line);
        static void  deallocateDebug(void* mem, const char* file, U32 line);
    };

    //**********************************************************************
    // Keeps track of all GLOBAL allocations / deallocations.
    //**********************************************************************
    class MemoryTracker
    {
        friend class _GlobalNewAndDeleteAllocator;
        MemoryTracker() {}

    public:
        ~MemoryTracker() { _CheckForMemoryLeak(); }

        //----------------------------------------------------------------------
        // Return the memory information struct. Information in 
        // that struct was gathered through global new/delete.
        // => Contains all allocations / deallocations.
        //----------------------------------------------------------------------
        static Memory::AllocationInfo& getAllocationMemoryInfo();

        //----------------------------------------------------------------------
        // Log the AllocationMemoryInfo
        //----------------------------------------------------------------------
        static void log();

    private:
#ifndef STATIC_LIB
        static MemoryTracker s_memoryLeakDetectionInstance;
#endif

        // Check for a memory leak. Halt the program if one detected.
        void _CheckForMemoryLeak();

        NULL_COPY_AND_ASSIGN(MemoryTracker)
    };


} } // end namespaces