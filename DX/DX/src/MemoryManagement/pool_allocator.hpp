#pragma once


/**********************************************************************
    class: PoolAllocator (pool_allocator.hpp)

    author: S. Hau
    date: October 4, 2017

    PoolAllocator interface. A pool-allocator preallocates a
    fixed amount of memory beforehand to AVOID dynamic
    memory allocation from the OS, which is in general very costly.
    Features:
    - The allocated memory is divided into equally sized blocks
    - Only memory blocks of those size can be allocated
    - This allows to deallocate arbitrarily
    - Fragmentation free
    Be careful about pointers pointing to memory in this allocator :-)
**********************************************************************/

namespace MemoryManagement
{
    //----------------------------------------------------------------------
    // Represents one allocatable chunk. If the chunk is not allocated it
    // contains a pointer to the next free chunk.
    //----------------------------------------------------------------------
    template <class T>
    union PoolChunk
    {
        T               value;
        PoolChunk<T>*   nextFreeChunk;

        PoolChunk() {};
        ~PoolChunk() {};
    };

    template <class T, class Allocator = std::allocator<PoolChunk<T>>>
    class PoolAllocator
    {
    public:
        //----------------------------------------------------------------------
        // @Params:
        // "amountOfChunks": Maximum number of chunks allocatable
        // "parentAllocator": Parent allocator from which this allocator pulls
        //                    his memory out
        //----------------------------------------------------------------------
        explicit PoolAllocator(Size amountOfChunks, Allocator* parentAllocator = nullptr);
        ~PoolAllocator();

        //----------------------------------------------------------------------
        // Allocates and constructs a new object of type T in this PoolAllocator.
        // @Params:
        // "args": Constructor arguments from the class T
        //----------------------------------------------------------------------
        template<class... Arguments>
        T* allocate(Arguments&&... args);

        //----------------------------------------------------------------------
        // Deallocates and deconstructs the given object in this PoolAllocator.
        // @Params:
        // "data": The object previously allocated from this pool.
        //----------------------------------------------------------------------
        void deallocate(T* data);

    private:
        PoolChunk<T>*   m_data = nullptr;
        PoolChunk<T>*   m_head = nullptr;

        Size            m_amountOfChunks = 0;
        Size            m_amountOfAllocatedChunks = 0;

        Allocator*      m_parentAllocator = nullptr;
        bool            m_deleteParentAllocator = false;

        bool _InMemoryRange(PoolChunk<T>* data){ return (data >= m_data) && (data < (m_data + m_amountOfChunks)); }

        PoolAllocator (const PoolAllocator& other) = delete;
        PoolAllocator& operator = (const PoolAllocator& other) = delete;
        PoolAllocator (const PoolAllocator&& other) = delete;
        PoolAllocator& operator = (const PoolAllocator&& other) = delete;
    };

    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************

    //----------------------------------------------------------------------
    template<class T, class Allocator>
    PoolAllocator<T, Allocator>::PoolAllocator(Size amountOfChunks, Allocator* parentAllocator)
        : m_amountOfChunks(amountOfChunks), m_parentAllocator(parentAllocator)
    {
        if (m_parentAllocator == nullptr)
        {
            m_parentAllocator = new Allocator();
            m_deleteParentAllocator = true;
        }

        m_data = m_parentAllocator->allocate( m_amountOfChunks );
        m_head = m_data;

        // Each block should point to its following block, except the last one
        for (Size i = 0; i < (m_amountOfChunks - 1); i++)
        {
            m_data[i].nextFreeChunk = std::addressof( m_data[i + 1] );
        }
        m_data[m_amountOfChunks - 1].nextFreeChunk = nullptr;
    }

    //----------------------------------------------------------------------
    template<class T, class Allocator>
    PoolAllocator<T, Allocator>::~PoolAllocator()
    {
        if (m_amountOfAllocatedChunks != 0)
            assert (false);

        m_parentAllocator->deallocate (m_data, m_amountOfChunks);

        if (m_deleteParentAllocator)
            delete m_parentAllocator;

        m_data = nullptr;
        m_head = nullptr;
        m_parentAllocator = nullptr;
    }

    //----------------------------------------------------------------------
    template<class T, class Allocator>
    template<class... Arguments>
    T* PoolAllocator<T, Allocator>::allocate(Arguments&&... args)
    {
        if (m_head == nullptr)
        {
            assert( false && "PoolAllocator: Out of memory" );
            return nullptr;
        }

        PoolChunk<T>* newChunk = m_head;
        m_head = m_head->nextFreeChunk;

        // Create a new object of type T using placement new and forward the constructor arguments
        T* retVal = new (std::addressof( newChunk->value )) T( std::forward<Arguments>(args)... );

        m_amountOfAllocatedChunks++;

        return retVal;
    }

    //----------------------------------------------------------------------
    template<class T, class Allocator>
    void PoolAllocator<T, Allocator>::deallocate(T* data)
    {
        // Check if given data was really allocated from this allocator
        PoolChunk<T>* chunkToDelete = reinterpret_cast<PoolChunk<T>*>( data );
        assert(_InMemoryRange(chunkToDelete));

        // Check if given chunk was already deallocated
        // The way this works is to check if the "nextFreeChunk" of the chunkToDelete points in
        // the valid memory range within this allocator
        assert(!_InMemoryRange(chunkToDelete->nextFreeChunk));

        data->~T();
        chunkToDelete->nextFreeChunk = m_head;
        m_head = chunkToDelete;

        m_amountOfAllocatedChunks--;
    }



}