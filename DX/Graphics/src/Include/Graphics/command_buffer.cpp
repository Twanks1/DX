#include "command_buffer.h"
/**********************************************************************
    class: CommandBuffer (command_buffer.cpp)

    author: S. Hau
    date: December 19, 2017
**********************************************************************/

#include "Logging/logging.h"
#include "i_material.h"
#include "i_shader.h"

namespace Graphics {

    //----------------------------------------------------------------------
    I32 getRenderQueue( const std::shared_ptr<GPUCommandBase>& cmd )
    {
        switch ( cmd->getType() )
        {
        case GPUCommand::DRAW_MESH:
        {
            auto drawCmd = std::reinterpret_pointer_cast<GPUC_DrawMesh>(cmd);
            return drawCmd->material->getShader()->getRenderQueue();
        }
        case GPUCommand::DRAW_MESH_INSTANCED:
        {
            auto drawCmd = std::reinterpret_pointer_cast<GPUC_DrawMeshInstanced>(cmd);
            return drawCmd->material->getShader()->getRenderQueue();
        }
        case GPUCommand::DRAW_MESH_SKINNED:
        {
            auto drawCmd = std::reinterpret_pointer_cast<GPUC_DrawMeshSkinned>(cmd);
            return drawCmd->material->getShader()->getRenderQueue();
        }
        case GPUCommand::DRAW_LIGHT:
            return -8196; // return low enough so it is always before every draw command
        }
        LOG_WARN_RENDERING( "CommandBuffer::getRenderQueue(): Expected draw command but was not! Consult your local programmer to fix this!" );
        return 0;
    }

    //----------------------------------------------------------------------
    CommandBuffer::CommandBuffer()
    {
        m_gpuCommands.reserve( COMMAND_BUFFER_INITIAL_CAPACITY );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::sortCommands()
    {
        std::sort( m_gpuCommands.begin(), m_gpuCommands.end(), [](const std::shared_ptr<Graphics::GPUCommandBase>& c1, const std::shared_ptr<Graphics::GPUCommandBase>& c2) {
            return c1->getType() < c2->getType();
        } );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::sortDrawCommands( const Math::Vec3& cameraPos )
    {
        // Sort draw commands by renderqueue first
        auto itBeginDraw = m_gpuCommands.begin();
        while ( itBeginDraw != m_gpuCommands.end() && ((*itBeginDraw)->getType() != GPUCommand::DRAW_MESH && 
                                                       (*itBeginDraw)->getType() != GPUCommand::DRAW_MESH_INSTANCED &&
                                                       (*itBeginDraw)->getType() != GPUCommand::DRAW_MESH_SKINNED &&
                                                       (*itBeginDraw)->getType() != GPUCommand::DRAW_LIGHT) )
            itBeginDraw++;

        auto itEndDraw = itBeginDraw;
        while ( itEndDraw != m_gpuCommands.end() && ((*itEndDraw)->getType() == GPUCommand::DRAW_MESH ||
                                                     (*itEndDraw)->getType() == GPUCommand::DRAW_MESH_INSTANCED || 
                                                     (*itEndDraw)->getType() == GPUCommand::DRAW_MESH_SKINNED || 
                                                     (*itEndDraw)->getType() == GPUCommand::DRAW_LIGHT) )
            itEndDraw++;

        std::sort( itBeginDraw, itEndDraw, [] (const std::shared_ptr<Graphics::GPUCommandBase>& c1, const std::shared_ptr<Graphics::GPUCommandBase>& c2) {
            return getRenderQueue( c1 ) < getRenderQueue( c2 );
        } );

        // All draw commands are now ordered properly e.g. drawmesh by renderqueue
        auto itBeginDrawTransparent = itBeginDraw;
        while (itBeginDrawTransparent != m_gpuCommands.end() && 
              ((*itBeginDrawTransparent)->getType() == GPUCommand::DRAW_MESH ||
               (*itBeginDrawTransparent)->getType() == GPUCommand::DRAW_MESH_INSTANCED ||
               (*itBeginDrawTransparent)->getType() == GPUCommand::DRAW_MESH_SKINNED ||
               (*itBeginDrawTransparent)->getType() == GPUCommand::DRAW_LIGHT) )
        {
            if ( getRenderQueue(*itBeginDrawTransparent) >= (I32)Graphics::RenderQueue::BackToFrontBoundary)
                break;
            itBeginDrawTransparent++;
        }

        // Sort transparent draw materials by camera distance
        std::sort( itBeginDrawTransparent, itEndDraw, [cameraPos](const std::shared_ptr<Graphics::GPUCommandBase>& c1, const std::shared_ptr<Graphics::GPUCommandBase>& c2) {
            auto d1 = std::reinterpret_pointer_cast<Graphics::GPUC_DrawMesh>( c1 );
            auto d2 = std::reinterpret_pointer_cast<Graphics::GPUC_DrawMesh>( c2 );
            auto pos = d1->modelMatrix.r[3];
            auto pos2 = d2->modelMatrix.r[3];

            auto camPos = DirectX::XMLoadFloat3( &cameraPos );
            auto distance1 = DirectX::XMVector4LengthSq( DirectX::XMVectorSubtract( camPos, pos ) );
            auto distance2 = DirectX::XMVector4LengthSq( DirectX::XMVectorSubtract( camPos, pos2 ) );

            return DirectX::XMVector4Greater( distance1, distance2 );
        } );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::merge( const CommandBuffer& cmd )
    {
        auto& commands = cmd.m_gpuCommands;
        m_gpuCommands.insert( m_gpuCommands.end(), commands.begin(), commands.end() );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::reset()
    {
        m_gpuCommands.clear();
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawMesh( const MeshPtr& mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex )
    {
        ASSERT( mesh && "Mesh is null, which is not allowed!" );
        ASSERT( material && "Material is null, which is not allowed!" );
        m_gpuCommands.push_back( std::make_unique<GPUC_DrawMesh>( mesh, material, modelMatrix, subMeshIndex ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawMeshInstanced( const MeshPtr& mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 instanceCount )
    {
        ASSERT( mesh && "Mesh is null, which is not allowed!" );
        ASSERT( material && "Material is null, which is not allowed!" );
        ASSERT( instanceCount > 0 && "Material is null, which is not allowed!" );
        m_gpuCommands.push_back( std::make_unique<GPUC_DrawMeshInstanced>( mesh, material, modelMatrix, instanceCount ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawMeshSkinned( const MeshPtr& mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex, const ArrayList<DirectX::XMMATRIX>& matrixPalette )
    {
        ASSERT( mesh && "Mesh is null, which is not allowed!" );
        ASSERT( material && "Material is null, which is not allowed!" );
        m_gpuCommands.push_back( std::make_unique<GPUC_DrawMeshSkinned>( mesh, material, modelMatrix, subMeshIndex, matrixPalette ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setCamera( const Camera& camera )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetCamera>( camera ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::endCamera()
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_EndCamera>() );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::copyTexture( const TexturePtr& srcTex, const TexturePtr& dstTex )
    {
        copyTexture( srcTex, 0, 0, dstTex, 0, 0 );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::copyTexture( const TexturePtr& srcTex, I32 srcElement, I32 srcMip, const TexturePtr& dstTex, I32 dstElement, I32 dstMip )
    {
        ASSERT( srcTex->getWidth() == dstTex->getWidth() && srcTex->getHeight() == dstTex->getHeight() && "Textures must be of same size" );
        m_gpuCommands.push_back( std::make_unique<GPUC_CopyTexture>( srcTex, srcElement, srcMip, dstTex, dstElement, dstMip ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawLight( const Light* light )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_DrawLight>( light ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setRenderTarget( const RenderTexturePtr& target )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetRenderTarget>( target ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawFullscreenQuad( const MaterialPtr& material )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_DrawFullscreenQuad>( material ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::renderCubemap( const CubemapPtr& cubemap, const MaterialPtr& material, I32 dstMip )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_RenderCubemap>( cubemap, material, dstMip ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::blit( const RenderTexturePtr& src, const RenderTexturePtr& dst, const MaterialPtr& material )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_Blit>( src, dst, material ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setScissor( const Math::Rect& rect )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetScissor>( rect ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setCameraMatrix( CameraMember member, const DirectX::XMMATRIX& matrix )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetCameraMatrix>( member, matrix ) );
    }

} // End namespaces