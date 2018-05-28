#pragma once
/**********************************************************************
    class: EnvironmentMap (environment_map.h)

    author: S. Hau
    date: May 27, 2018

    Environment-Mapping after UE4, which splits the IBL in a diffuse 
    irradiance map and a specular reflection map.
**********************************************************************/

#include "Graphics/i_cubemap.hpp"

namespace Core { namespace Assets { 

    //*********************************************************************
    class EnvironmentMap
    {
    public:
        EnvironmentMap(const CubemapPtr& cubemap, I32 diffuseIrradianceSize = 64, I32 specularReflectionSize = 128);

        //----------------------------------------------------------------------
        const CubemapPtr& getDiffuseIrradianceMap()  const { return m_diffuseIrradianceMap; }
        const CubemapPtr& getSpecularReflectionMap() const { return m_specularReflectionMap; }

    private:
        CubemapPtr m_diffuseIrradianceMap;
        CubemapPtr m_specularReflectionMap;

        //----------------------------------------------------------------------
        EnvironmentMap(const EnvironmentMap& other)               = delete;
        EnvironmentMap& operator = (const EnvironmentMap& other)  = delete;
        EnvironmentMap(EnvironmentMap&& other)                    = delete;
        EnvironmentMap& operator = (EnvironmentMap&& other)       = delete;
    };

} } // End namespaces