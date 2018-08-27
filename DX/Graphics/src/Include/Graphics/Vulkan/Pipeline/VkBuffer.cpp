#include "VkBuffer.h"
/**********************************************************************
    class: Buffer

    author: S. Hau
    date: August 27, 2018
**********************************************************************/

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    // BUFFER
    //**********************************************************************

    //----------------------------------------------------------------------
    void Buffer::create( const void* data, U32 sizeInBytes, BufferUsage updateFreq, VkBufferUsageFlags vkUsageFlags )
    {
        ASSERT( buffer == VK_NULL_HANDLE && "Buffer was already created." );

        this->size  = sizeInBytes;
        this->usage = updateFreq;

        VezMemoryFlagsBits memType = VEZ_MEMORY_GPU_ONLY;

        VezBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.usage = vkUsageFlags;
        switch (usage)
        {
        case BufferUsage::Immutable:
        case BufferUsage::LongLived:
            vkUsageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT; memType = VEZ_MEMORY_GPU_ONLY;  break;
        case BufferUsage::Frequently:
            memType = VEZ_MEMORY_CPU_TO_GPU;
            break;
        }

        bufferCreateInfo.size = sizeInBytes;
        VALIDATE( vezCreateBuffer( g_vulkan.device, memType, &bufferCreateInfo, &buffer ) );

        // Upload the data to gpu
        update( data, sizeInBytes );
    }

    //----------------------------------------------------------------------
    void Buffer::update( const void* data, U32 sizeInBytes )
    {
        ASSERT( buffer && "Buffer does not exist." );
        ASSERT( sizeInBytes <= size && "Update size too large!" );

        switch (usage)
        {
        case BufferUsage::Immutable:
        case BufferUsage::LongLived:
            VALIDATE( vezBufferSubData( g_vulkan.device, buffer, 0, sizeInBytes, data) );
            break;
        case BufferUsage::Frequently:
        {
            void* pData;
            vezMapBuffer( g_vulkan.device, buffer, 0, VK_WHOLE_SIZE, &pData );
            memcpy( pData, data, sizeInBytes );
            vezUnmapBuffer( g_vulkan.device, buffer );
            break;
        } 
        }
    }

    //----------------------------------------------------------------------
    void Buffer::destroy()
    {
        if (buffer)
        {
            vezDestroyBuffer( g_vulkan.device, buffer );
            buffer = VK_NULL_HANDLE;
            size = 0;
            usage = BufferUsage::Unknown;
        }
    }

    //**********************************************************************
    // VERTEXBUFFER
    //**********************************************************************

    //----------------------------------------------------------------------
    void VertexBuffer::create( const void* data, U32 sizeInBytes, BufferUsage usage )
    {
        Buffer::create( data, sizeInBytes, usage, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT );
    }

    //----------------------------------------------------------------------
    void IndexBuffer::create( const void* data, U32 sizeInBytes, BufferUsage usage )
    {
        Buffer::create( data, sizeInBytes, usage, VK_BUFFER_USAGE_INDEX_BUFFER_BIT );
    }

    //----------------------------------------------------------------------
    void UniformBuffer::create( const void* data, U32 sizeInBytes, BufferUsage usage )
    {
        Buffer::create( data, sizeInBytes, usage, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT );
    }

} } // End namespaces