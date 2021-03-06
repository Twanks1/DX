#pragma once

/**********************************************************************
    class: MemoryManager (memory_manager.h)

    author: S. Hau
    date: October 12, 2017

    Reports memory leaks on shutdown.
    @Considerations
      - Allocations from Allocators fetch there memory from a
        universalalloctor in this class?
         => Preallocate a large chunk of memory
**********************************************************************/

#include "Common/i_subsystem.hpp"
#include "Memory/memory_structs.h"


namespace Core { namespace MemoryManagement{

    //*********************************************************************
    // MemoryManager class, which tracks memory allocations.
    //*********************************************************************
    class MemoryManager : public ISubSystem
    {
    public:
        MemoryManager() {}

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void OnTick(Time::Seconds delta) override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // Log the current allocation info to the console.
        //----------------------------------------------------------------------
        void log();

        //----------------------------------------------------------------------
        // @Return:
        //   Contains all allocations made in this program.
        //----------------------------------------------------------------------
        const Memory::AllocationInfo getAllocationInfo() const;

    private:
        //----------------------------------------------------------------------
        void _ReportPossibleMemoryLeak(const Memory::AllocationInfo& lastAllocationInfo, const Memory::AllocationInfo& allocationInfo);

        // List here different strategies for memory leak detection
        void _ContinousAllocationLeakDetection();
        void _BasicLeakDetection();

        NULL_COPY_AND_ASSIGN(MemoryManager)
    };






} }