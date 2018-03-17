#include "D3D11Mesh.h"
/**********************************************************************
    class: Mesh (D3D11Mesh.cpp)

    author: S. Hau
    date: March 8, 2018
**********************************************************************/

#include "../Pipeline/Buffers/D3D11Buffers.h"
#include "../../structs.hpp"
#include "D3D11Shader.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    Mesh::~Mesh()
    {
        clear();
    }

    //----------------------------------------------------------------------
    void Mesh::bind( const VertexLayout& vertLayout, U32 subMeshIndex )
    {
        _SetTopology( subMeshIndex );
        _BindVertexBuffer( vertLayout, subMeshIndex ); 
        _BindIndexBuffer( subMeshIndex );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Mesh::clear()
    {
        if (m_pVertexBuffer)
            SAFE_DELETE( m_pVertexBuffer );
        if (m_pColorBuffer)
            SAFE_DELETE( m_pColorBuffer );
        for (auto& indexBuffer : m_pIndexBuffers)
            SAFE_DELETE( indexBuffer );

        m_vertices.clear();
        m_vertexColors.clear();
        m_pIndexBuffers.clear();
        m_subMeshes.clear();
    }

    //----------------------------------------------------------------------
    void Mesh::setVertices( const ArrayList<Math::Vec3>& vertices )
    {
#if _DEBUG
        if (m_pVertexBuffer != nullptr)
            ASSERT( (m_vertices.size() == vertices.size() &&
                    "IMesh::setVertices(): The amount of vertices given must be the number of vertices already present! "
                    "Otherwise call clear() before, so the gpu buffer will be recreated.") );
#endif
        m_vertices = vertices;
        if (m_pVertexBuffer == nullptr)
        {
            m_pVertexBuffer = new VertexBuffer( m_vertices.data(), getVertexCount() * sizeof( Math::Vec3 ), m_bufferUsage );
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

            m_pVertexBuffer->update( m_vertices.data(), m_vertices.size() * sizeof( Math::Vec3 ) );
        }
    }

    //----------------------------------------------------------------------
    void Mesh::setIndices( const ArrayList<U32>& indices, U32 subMeshIndex, MeshTopology topology, U32 baseVertex )
    {
#if _DEBUG
        if ( !m_subMeshes.empty() )
        {
            if ( subMeshIndex < m_pIndexBuffers.size() )
            {
                ASSERT( m_subMeshes[subMeshIndex].indices.size() == indices.size() &&
                        "IMesh::setTriangles(): The amount of indices given must be the number of indices already present for the given submesh! "
                        "Otherwise call clear() before, so the gpu buffer will be recreated." );
            }
            else // Check if subMeshIndex is valid
            {
                String errorMessage = "The submesh index is invalid. It must be in in ascending order."
                                      " The next index would be: " + TS( m_pIndexBuffers.size() );
                ASSERT( subMeshIndex == m_pIndexBuffers.size() && "Invalid submesh index." );
            }
        }
#endif
        if ( not hasSubMesh( subMeshIndex ) )
        {
            auto& sm = AddSubMesh( indices, topology, baseVertex );

            switch ( sm.indexFormat )
            {
                case IndexFormat::U16:
                {
                    ArrayList<U16> indicesU16;
                    for ( auto& index : sm.indices )
                        indicesU16.push_back( index );
                    auto pIndexBuffer = new D3D11::IndexBuffer( indicesU16.data(), getIndexCount( subMeshIndex ) * sizeof( U16 ), m_bufferUsage );
                    m_pIndexBuffers.push_back( pIndexBuffer );
                    break;
                }
                case IndexFormat::U32:
                {
                    auto pIndexBuffer = new D3D11::IndexBuffer( sm.indices.data(), getIndexCount( subMeshIndex ) * sizeof( U32 ), m_bufferUsage );
                    m_pIndexBuffers.push_back( pIndexBuffer );
                    break;
                }
            }
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );
            auto& sm = m_subMeshes[subMeshIndex];
            sm.indices      = indices;
            sm.baseVertex   = baseVertex;

            // Update index-buffer
            Size indexSize = 0;
            switch ( getIndexFormat( subMeshIndex ) )
            {
            case IndexFormat::U16: indexSize = sizeof( U16 ); break;
            case IndexFormat::U32: indexSize = sizeof( U32 ); break;
            }
            m_pIndexBuffers[subMeshIndex]->update( sm.indices.data(), sm.indices.size() * indexSize );
        }
    }

    //----------------------------------------------------------------------
    void Mesh::setColors( const ArrayList<Color>& colors )
    {
        if (m_pColorBuffer != nullptr)
            ASSERT( m_vertexColors.size() == colors.size() &&
                "IMesh::setColors(): The amount of colors given must be the number of colors already present! "
                "Otherwise call clear() before, so the gpu buffer will be recreated.");

        m_vertexColors = colors;
        if (m_pColorBuffer == nullptr)
        {
            ArrayList<F32> colorsNormalized( m_vertexColors.size() * 4 );
            for (U32 i = 0; i < m_vertexColors.size(); i++)
            {
                auto normalized = m_vertexColors[i].normalized();
                colorsNormalized[i * 4 + 0] = normalized[0];
                colorsNormalized[i * 4 + 1] = normalized[1];
                colorsNormalized[i * 4 + 2] = normalized[2];
                colorsNormalized[i * 4 + 3] = normalized[3];
            }
            m_pColorBuffer = new VertexBuffer( colorsNormalized.data(), static_cast<U32>( colorsNormalized.size() ) * sizeof( F32 ), m_bufferUsage );
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

            ArrayList<F32> colorsNormalized( m_vertexColors.size() * 4 );
            for (U32 i = 0; i < m_vertexColors.size(); i++)
            {
                auto normalized = m_vertexColors[i].normalized();
                colorsNormalized[i * 4 + 0] = normalized[0];
                colorsNormalized[i * 4 + 1] = normalized[1];
                colorsNormalized[i * 4 + 2] = normalized[2];
                colorsNormalized[i * 4 + 3] = normalized[3];
            }
            m_pColorBuffer->update( colorsNormalized.data(), static_cast<U32>( colorsNormalized.size() ) * sizeof( F32 ) );
        }
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Mesh::recreateBuffers()
    {
        // Recreate vertex buffer
        if (m_pVertexBuffer)
        {
            SAFE_DELETE( m_pVertexBuffer );
            setVertices( m_vertices );
        }

        // Recreate color buffer
        if (m_pColorBuffer)
        {
            SAFE_DELETE( m_pColorBuffer );
            setColors( m_vertexColors );
        }

        // Recreate index buffers
        for (auto& indexBuffer : m_pIndexBuffers)
            SAFE_DELETE( indexBuffer );
        m_pIndexBuffers.clear();

        auto subMeshes = m_subMeshes;
        m_subMeshes.clear();
        for (U32 i = 0; i < subMeshes.size(); i++)
            setIndices( subMeshes[i].indices, i, subMeshes[i].topology, subMeshes[i].baseVertex );
    }

    //----------------------------------------------------------------------
    void Mesh::_SetTopology( U32 subMeshIndex )
    {
        D3D_PRIMITIVE_TOPOLOGY dxTopology;
        switch (m_subMeshes[subMeshIndex].topology)
        {
        case MeshTopology::Lines:       dxTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break;
        case MeshTopology::LineStrip:   dxTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
        case MeshTopology::Points:      dxTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST; break;
        case MeshTopology::Quads: ASSERT(false && "checkThis"); dxTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
        case MeshTopology::Triangles:   dxTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
        }
        g_pImmediateContext->IASetPrimitiveTopology( dxTopology );
    }

    //----------------------------------------------------------------------
    void Mesh::_BindVertexBuffer( const VertexLayout& vertLayout, U32 subMesh )
    {
        auto& vertexDescription = vertLayout.getLayoutDescription();

        ArrayList<ID3D11Buffer*> pBuffers;
        ArrayList<U32>           strides;
        ArrayList<U32>           offsets;
        for ( auto& binding : vertexDescription )
        {
            switch (binding.type)
            {
            case InputLayoutType::POSITION:
            {
                pBuffers.emplace_back( m_pVertexBuffer->getBuffer() );
                strides.emplace_back( static_cast<U32>( sizeof( Math::Vec3 ) ) );
                offsets.emplace_back( 0 );
                break;
            }
            case InputLayoutType::COLOR:
            {
                pBuffers.emplace_back( m_pColorBuffer->getBuffer() );
                strides.emplace_back( static_cast<U32>( sizeof( F32 ) * 4 ) );
                offsets.emplace_back( 0 );
                break;
            }
            default:
                ASSERT( false && "implement!" );
            }
        }

        // Bind vertex buffers
        g_pImmediateContext->IASetVertexBuffers( 0, static_cast<U32>( pBuffers.size() ), pBuffers.data(), strides.data(), offsets.data() );
    }

    //----------------------------------------------------------------------
    void Mesh::_BindIndexBuffer( U32 subMeshIndex )
    {
        switch (m_subMeshes[subMeshIndex].indexFormat)
        {
        case IndexFormat::U16: m_pIndexBuffers[subMeshIndex]->bind( DXGI_FORMAT_R16_UINT, 0 ); break;
        case IndexFormat::U32: m_pIndexBuffers[subMeshIndex]->bind( DXGI_FORMAT_R32_UINT, 0 ); break;
        }
    }


} } // End namespaces