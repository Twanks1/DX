#include "debug_manager.h"
/**********************************************************************
    class: DebugManager (debug_manager.cpp)

    author: S. Hau
    date: April 2, 2018

    @Consideration:
     - Create one mesh for each material and update only mesh data,
       so everything will be just drawn with one draw-call
**********************************************************************/

#include "Core/locator.h"
#include "Events/event_dispatcher.h"
#include "Core/event_names.hpp"
#include "GameplayLayer/i_scene.h"
#include "Core/Assets/mesh_generator.h"
#include "../Resources/default_shaders.hpp"

namespace Core { namespace Debug {

    //----------------------------------------------------------------------
    void DebugManager::init()
    {
        // Debug manager needs to tick before the scene manager
        Locator::getCoreEngine().subscribe( this, true );

        // Register to switch scene event
        Events::Event& evt = Events::EventDispatcher::GetEvent( EVENT_SCENE_CHANGED );
        evt.addListener( BIND_THIS_FUNC_0_ARGS( &DebugManager::_OnSceneChanged ) );

        // Create both shaders with / withot depth-test
        m_colorShaderWireframe = RESOURCES.createShader();
        m_colorShaderWireframe->setName( "DEBUG - DEPTH" );
        m_colorShaderWireframe->setRasterizationState( { Graphics::FillMode::Wireframe, Graphics::CullMode::None } );
        m_colorShaderWireframe->compileFromSource( Resources::COLOR_VERTEX_SHADER_SOURCE, Resources::COLOR_FRAGMENT_SHADER_SOURCE, "main" );

        m_colorShaderWireframeNoDepthTest = RESOURCES.createShader();
        m_colorShaderWireframeNoDepthTest->setName("DEBUG - NO DEPTH");
        m_colorShaderWireframeNoDepthTest->setRasterizationState( { Graphics::FillMode::Wireframe, Graphics::CullMode::None } );
        m_colorShaderWireframeNoDepthTest->setDepthStencilState( { false } );
        m_colorShaderWireframeNoDepthTest->compileFromSource( Resources::COLOR_VERTEX_SHADER_SOURCE, Resources::COLOR_FRAGMENT_SHADER_SOURCE, "main" );

        // Create material from both shaders
        m_colorMaterial = RESOURCES.createMaterial( m_colorShaderWireframe );
        m_colorMaterialNoDepthTest = RESOURCES.createMaterial( m_colorShaderWireframeNoDepthTest );
    }

    //----------------------------------------------------------------------
    void DebugManager::OnTick( Time::Seconds delta )
    {
        bool erasedMesh = false;
        for (auto it = m_currentMeshes.begin(); it != m_currentMeshes.end();)
        {
            it->duration -= delta;
            if (it->duration < 0)
            {
                erasedMesh = true;
                it = m_currentMeshes.erase( it );
            }
            else
            {
                it++;
            }
        }

        // If a mesh was removed, update command buffer list
        if (erasedMesh)
            _UpdateCommandBuffer();
    }

    //----------------------------------------------------------------------
    void DebugManager::shutdown()
    {
        m_currentMeshes.clear();
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void DebugManager::drawLine( const Math::Vec3& start, const Math::Vec3& end, Color color, Time::Seconds duration, bool depthTest )
    {
        auto mesh = RESOURCES.createMesh();
        mesh->setVertices( { start, end } );
        mesh->setIndices( { 0, 1 }, 0, Graphics::MeshTopology::Lines );
        mesh->setColors( { color, color } );

        _AddMesh( mesh, duration, depthTest );
    }

    //----------------------------------------------------------------------
    void DebugManager::drawRay( const Math::Vec3& start, const Math::Vec3& direction, Color color, Time::Seconds duration, bool depthTest )
    {
        auto mesh = RESOURCES.createMesh();
        mesh->setVertices( { start, start + direction } );
        mesh->setIndices( { 0, 1 }, 0, Graphics::MeshTopology::Lines );
        mesh->setColors( { color, color } );

        _AddMesh( mesh, duration, depthTest );
    }

    //----------------------------------------------------------------------
    void DebugManager::drawCube( const Math::Vec3& min, const Math::Vec3& max, Color color, Time::Seconds duration, bool depthTest )
    {
        // Line for each edge
        ArrayList<Math::Vec3> vertices =
        {
            min,
            Math::Vec3( min.x, min.y, max.z ),
            Math::Vec3( max.x, min.y, min.z ),
            Math::Vec3( min.x, max.y, min.z ),
            max,
            Math::Vec3( max.x, max.y, min.z ),
            Math::Vec3( min.x, max.y, max.z ),
            Math::Vec3( max.x, min.y, max.z ),
        };
        ArrayList<U32> indices = {
            0, 1,
            0, 2,
            0, 3,
            4, 5,
            4, 6,
            4, 7,
            1, 6,
            3, 6,
            2, 7,
            2, 5,
            1, 7,
            3, 5
        };

        auto mesh = RESOURCES.createMesh();
        mesh->setVertices( vertices );
        mesh->setIndices( indices, 0, Graphics::MeshTopology::Lines );
        mesh->setColors( ArrayList<Color>( indices.size(), color ) );

        _AddMesh( mesh, duration, depthTest );
    }

    //----------------------------------------------------------------------
    void DebugManager::drawCube( const Math::AABB& aabb, Color color, Time::Seconds duration, bool depthTest )
    {
        drawCube( aabb.getMin(), aabb.getMax(), color, duration, depthTest );
    }

    //----------------------------------------------------------------------
    void DebugManager::drawSphere( const Math::Vec3& pos, F32 radius, Color color, Time::Seconds duration, bool depthTest )
    {
        auto mesh = Assets::MeshGenerator::CreateUVSphere( pos, radius, 30, 30 );       
        mesh->setColors(  ArrayList<Color>( mesh->getIndexCount( 0 ), color ) );

        _AddMesh( mesh, duration, depthTest );
    }

    //----------------------------------------------------------------------
    void DebugManager::drawFrustum( const Math::Vec3& pos, const Math::Vec3& up, const Math::Vec3& right, const Math::Vec3& forward, 
                                    F32 fovAngleYDeg, F32 zNear, F32 zFar, F32 aspectRatio,
                                    Color color, Time::Seconds duration, bool depthTest )
    {        
        auto mesh = Assets::MeshGenerator::CreateFrustum( pos, up, right, forward, fovAngleYDeg, zNear, zFar, aspectRatio, color );
        _AddMesh( mesh, duration, depthTest );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void DebugManager::_UpdateCommandBuffer()
    {
        // Clear all previous commands
        m_commandBuffer.reset();

        // Record new commands
        for (auto& meshInfo : m_currentMeshes)
            m_commandBuffer.drawMesh( meshInfo.mesh, meshInfo.depthTest ? m_colorMaterial : m_colorMaterialNoDepthTest, DirectX::XMMatrixIdentity(), 0 );
    }

    //----------------------------------------------------------------------
    void DebugManager::_OnSceneChanged()
    {
        // Problem with this: If the scene gets loaded on a different thread and this scene has a 
        // drawing request in his init methods it deletes those meshes immediately
        //m_currentMeshes.clear();
        //_UpdateCommandBuffer();

        // Re-Add the command buffer to the new cameras in the scene
        for ( auto& cam : SCENE.getComponentManager().getCameras() )
            cam->addCommandBuffer( &m_commandBuffer );
    }

    //----------------------------------------------------------------------
    void DebugManager::_AddMesh( MeshPtr mesh, Time::Seconds duration, bool depthTest )
    {
        MeshInfo meshInfo;
        meshInfo.mesh       = mesh;
        meshInfo.duration   = duration;
        meshInfo.depthTest  = depthTest;
        m_currentMeshes.push_back( meshInfo );

        m_commandBuffer.drawMesh( mesh, depthTest ? m_colorMaterial : m_colorMaterialNoDepthTest, DirectX::XMMatrixIdentity(), 0 );
    }

} } // end namespaces