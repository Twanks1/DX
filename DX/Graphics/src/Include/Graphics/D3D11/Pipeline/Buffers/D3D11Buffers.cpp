#include "D3D11Buffers.h"
/**********************************************************************
    class: VertexBuffer, IndexBuffer, ConstantBuffer (D3D11Buffers.cpp)

    author: S. Hau
    date: December 6, 2017
**********************************************************************/

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    // VertexBuffer
    //**********************************************************************

    //----------------------------------------------------------------------
    VertexBuffer::VertexBuffer( const void* pData, U32 size, BufferUsage usage )
        : IBuffer( D3D11_BIND_VERTEX_BUFFER, usage, size, pData )
    {
    }

    //----------------------------------------------------------------------
    void VertexBuffer::bind( U32 slot, U32 stride, U32 offset )
    {
        g_pImmediateContext->IASetVertexBuffers( slot, 1, &m_pBuffer, &stride, &offset );
    }

    //**********************************************************************
    // IndexBuffer
    //**********************************************************************

    //----------------------------------------------------------------------
    IndexBuffer::IndexBuffer( const void* pData, U32 size, BufferUsage usage )
        : IBuffer( D3D11_BIND_INDEX_BUFFER, usage, size, pData )
    {
    }

    //----------------------------------------------------------------------
    void IndexBuffer::bind( DXGI_FORMAT format, U32 offset )
    {
        g_pImmediateContext->IASetIndexBuffer( m_pBuffer, format, offset );
    }

    //**********************************************************************
    // ConstantBuffer
    //**********************************************************************

    //----------------------------------------------------------------------
    ConstantBuffer::ConstantBuffer( U32 size, BufferUsage usage )
        : IBuffer( D3D11_BIND_CONSTANT_BUFFER, usage, size )
    {
    }

    //----------------------------------------------------------------------
    void ConstantBuffer::bindToVertexShader( U32 slot ) const
    {
        g_pImmediateContext->VSSetConstantBuffers( slot, 1, &m_pBuffer );
    }

    //----------------------------------------------------------------------
    void ConstantBuffer::bindToPixelShader( U32 slot ) const
    {
        g_pImmediateContext->PSSetConstantBuffers( slot, 1, &m_pBuffer );
    }

    //----------------------------------------------------------------------
    void ConstantBuffer::bindToGeometryShader( U32 slot ) const
    {
        g_pImmediateContext->GSSetConstantBuffers( slot, 1, &m_pBuffer );
    }

} } // End namespaces