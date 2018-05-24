#include "spot_light.h"
/**********************************************************************
    class: SpotLight (spot_light.h)
    
    author: S. Hau
    date: May 22, 2018
**********************************************************************/

#include "Graphics/command_buffer.h"
#include "GameplayLayer/gameobject.h"

namespace Components {

    //----------------------------------------------------------------------
    SpotLight::SpotLight( F32 intensity, Color color, F32 spotAngleInDegrees, F32 range )
        : m_spotLight( intensity, color, { 0, 1, 0 }, spotAngleInDegrees, { 0, 0, 1 }, range )
    {
    }

    //----------------------------------------------------------------------
    void SpotLight::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        m_spotLight.setPosition( transform->getWorldPosition());
        m_spotLight.setDirection( transform->getWorldRotation().getForward() );

        cmd.drawLight( &m_spotLight );
    }

    //----------------------------------------------------------------------
    bool SpotLight::cull( const Camera& camera )
    { 
        //@TODO:
        return true; 
    }


} // End namespaces