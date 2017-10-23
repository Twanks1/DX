#include "memory_structs.h"

/**********************************************************************
    class: None (memory_structs.cpp)

    author: S. Hau
    date: October 12, 2017

**********************************************************************/

#include "Utils/utils.h"

namespace Core { namespace MemoryManagement {

    static std::mutex mutex;

    //----------------------------------------------------------------------
    void AllocationMemoryInfo::addAllocation( Size amtOfBytes )
    {
        std::lock_guard<std::mutex> lock( mutex );

        currentBytesAllocated += amtOfBytes;
        totalBytesAllocated += amtOfBytes;
        totalAllocations++;
    }

    //----------------------------------------------------------------------
    void AllocationMemoryInfo::removeAllocation( Size amtOfBytes )
    {
        std::lock_guard<std::mutex> lock(mutex);

        ASSERT( totalDeallocations != totalAllocations );
        currentBytesAllocated -= amtOfBytes;
        totalBytesFreed += amtOfBytes;
        totalDeallocations++;
    }

    //----------------------------------------------------------------------
    String AllocationMemoryInfo::toString() const
    {
        String result;
        result.reserve( 255 );

        result += ( "\n-------------- MEMORY INFO ---------------");
        result += ( "\nCurrent Allocated: " + Utils::bytesToString( currentBytesAllocated ) );
        result += ( "\nTotal Allocated: " + Utils::bytesToString( totalBytesAllocated ) );
        result += ( "\nTotal Freed: " + Utils::bytesToString( totalBytesFreed ) );
        result += ( "\nTotal Allocations: " + TS( totalAllocations ) );
        result += ( "\nTotal Deallocations: " + TS( totalDeallocations ) );
        result += ( "\n------------------------------------------\n" );

        return result;
    }

    //----------------------------------------------------------------------
    AllocationMemoryInfo  AllocationMemoryInfo::operator - (const AllocationMemoryInfo& other) const
    {
        AllocationMemoryInfo result = {};

        result.currentBytesAllocated = currentBytesAllocated - other.currentBytesAllocated;
        result.totalBytesAllocated   = totalBytesAllocated - other.totalBytesAllocated;
        result.totalBytesFreed       = totalBytesFreed - other.totalBytesFreed;
        result.totalAllocations      = totalAllocations - other.totalAllocations;
        result.totalDeallocations    = totalDeallocations - other.totalDeallocations;

        return result;
    }

    //----------------------------------------------------------------------
    AllocationMemoryInfo  AllocationMemoryInfo::operator + (const AllocationMemoryInfo& other) const
    {
        AllocationMemoryInfo result = {};

        result.currentBytesAllocated = currentBytesAllocated + other.currentBytesAllocated;
        result.totalBytesAllocated   = totalBytesAllocated + other.totalBytesAllocated;
        result.totalBytesFreed       = totalBytesFreed + other.totalBytesFreed;
        result.totalAllocations      = totalAllocations + other.totalAllocations;
        result.totalDeallocations    = totalDeallocations + other.totalDeallocations;

        return result;
    }

} } // end namespaces