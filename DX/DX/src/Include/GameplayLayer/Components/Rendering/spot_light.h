#pragma once
/**********************************************************************
    class: SpotLight (spot_light.h)
    
    author: S. Hau
    date: May 22, 2018

    Classical spot light.
    The position/rotation is determined based on the attached transform component.
**********************************************************************/

#include "i_render_component.hpp"
#include "Graphics/Lighting/spot_light.h"

namespace Components {

    //**********************************************************************
    class SpotLight : public IRenderComponent
    {
    public:
        //----------------------------------------------------------------------
        SpotLight(F32 intensity = 1.0f, Color color = Color::WHITE, F32 spotAngleInDegrees = 45.0f, F32 range = 10.0f);

        //----------------------------------------------------------------------
        F32     getIntensity()  const { return m_spotLight.getIntensity(); }
        Color   getColor()      const { return m_spotLight.getColor(); }
        F32     getRange()      const { return m_spotLight.getRange(); }
        F32     getAngle()      const { return Math::Rad2Deg( m_spotLight.getAngle() ); }

        //----------------------------------------------------------------------
        void setIntensity   (F32 intensity) { m_spotLight.setIntensity(intensity); }
        void setColor       (Color color)   { m_spotLight.setColor(color); }
        void setRange       (F32 range)     { m_spotLight.setRange(range); }
        void setAngle       (F32 angle)     { m_spotLight.setAngle(angle); }

    private:
        Graphics::SpotLight m_spotLight;

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Camera& camera) override;

        //----------------------------------------------------------------------
        SpotLight(const SpotLight& other)               = delete;
        SpotLight& operator = (const SpotLight& other)  = delete;
        SpotLight(SpotLight&& other)                    = delete;
        SpotLight& operator = (SpotLight&& other)       = delete;
    };


} // End namespaces