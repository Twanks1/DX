#include "gameobject.h"
/**********************************************************************
    class: GameObject (gameobject.cpp)

    author: S. Hau
    date: December 17, 2017
**********************************************************************/

#include "i_scene.h"

//----------------------------------------------------------------------
GameObject::GameObject( IScene* scene, CString name )
    : m_attachedScene( scene ), m_name( name, true ) 
{
}

//----------------------------------------------------------------------
GameObject::~GameObject()
{
    for (auto& pair : m_components)
    {
        pair.second->shutdown();
        m_attachedScene->getComponentManager().Destroy( pair.second );
        SAFE_DELETE( pair.second );
    }
}

//**********************************************************************
// PUBLIC
//**********************************************************************

//**********************************************************************
// PRIVATE
//**********************************************************************

//----------------------------------------------------------------------
void GameObject::_PreTick( Time::Seconds delta )
{
    // Update components from all game-objects
    for ( auto& pair : m_components )
    {
        auto& comp = pair.second;
        if ( comp->isActive() )
        {
            if ( !comp->m_bInitialized )
            {
                comp->init();
                comp->m_bInitialized = true;
            }
            comp->preTick( delta );
        }
    }
}

//----------------------------------------------------------------------
void GameObject::_Tick( Time::Seconds delta )
{
    // Update components from all game-objects
    for (auto& pair : m_components)
    {
        auto& comp = pair.second;
        if ( comp->isActive() )
            comp->tick( delta );
    }
}

//----------------------------------------------------------------------
void GameObject::_LateTick( Time::Seconds delta )
{
    // Update components from all game-objects
    for (auto& pair : m_components)
    {
        auto& comp = pair.second;
        if ( comp->isActive() )
            comp->lateTick( delta );
    }
}