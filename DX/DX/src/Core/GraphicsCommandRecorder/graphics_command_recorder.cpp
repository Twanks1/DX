#include "graphics_command_recorder.h"
/**********************************************************************
    class: GraphicsRecorder (graphics_recorder.cpp)

    author: S. Hau
    date: December 24, 2017
**********************************************************************/

#include "locator.h"
#include "GameplayLayer/i_scene.h"
#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"
#include "GameplayLayer/Components/Rendering/camera.h"
#include "GameplayLayer/Components/Rendering/c_renderer.h"

namespace Core {

    //----------------------------------------------------------------------
    void GraphicsCommandRecorder::init()
    {
        for(I32 i = 0; i < COMMAND_BUFFER_COUNT; i++)
            m_CommandBuffers[i] = new Graphics::CommandBuffer();
    }

    //----------------------------------------------------------------------
    void GraphicsCommandRecorder::shutdown()
    {
        for (I32 i = 0; i < COMMAND_BUFFER_COUNT; i++)
            SAFE_DELETE( m_CommandBuffers[i] );
    }

    //----------------------------------------------------------------------
    void GraphicsCommandRecorder::render( IScene& scene, F32 lerp )
    {
        auto& graphicsEngine = Locator::getRenderer();

        // Fetch all renderer components e.g. model-renderer
        auto& renderers = Components::CRenderer::GetAll();

        auto& cmd = m_CommandBuffers[0];
        cmd->reset();

        auto& cameras = Components::Camera::GetAll();
        for (auto& cam : cameras)
        {
            // @TODO: Where store command buffer per camera?
            cam->recordGraphicsCommands( *cmd, lerp );

            // Add optionally attached command buffer aswell
            //foreach commandBuffer in camera->commandBuffers
                //graphicsEngine->dispatch(commandBuffer)

            // Do viewfrustum culling with every renderer component and THIS CAMERA
            for (auto& renderer : renderers)
            {
                if ( !renderer->isActive() )
                    continue;
                //bool isVisible = renderer->Cull(camera);
                //bool layerMatch = camera->layerMask & renderer->getLayerMask();
                //if (isVisible && layerMatch)
                     renderer->recordGraphicsCommands( *cmd, lerp );
            }

            // Execute rendering commands
            graphicsEngine.dispatch( *m_CommandBuffers[0] );
        }

        //auto& gameObjects = scene.getGameObjects();
        //for ( auto go : gameObjects )
        //{
        //    if ( go->isActive() )
        //    {
        //        go->recordGraphicsCommands( *m_CommandBuffers[0], lerp );
        //    }
        //}
    }

}