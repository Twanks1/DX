#include "environment_map.h"
/**********************************************************************
    class: EnvironmentMap (environment_map.cpp)

    author: S. Hau
    date: May 27, 2018
**********************************************************************/

#include "Core/locator.h"

namespace Assets {

    #define SHADER_NAME_ENV_MAP         "environmentMap"
    #define SHADER_NAME_SPEC_SAMPLES    "numSamples"
    #define SHADER_NAME_RESOLUTION      "resolution"
    #define SHADER_NAME_ROUGHNESS       "roughness"

    //----------------------------------------------------------------------
    EnvironmentMap::EnvironmentMap( const CubemapPtr& cubemap, I32 diffuseIrradianceSize, I32 specularReflectionSize )
    {
        cubemap->setFilter( Graphics::TextureFilter::Trilinear );
        cubemap->setClampMode( Graphics::TextureAddressMode::Clamp );

        // Diffuse irradiance map
        {
            Graphics::CommandBuffer cmd;
            m_diffuseIrradianceMap = RESOURCES.createCubemap();
            m_diffuseIrradianceMap->create( diffuseIrradianceSize, Graphics::TextureFormat::RGBAFloat );

            auto shader = ASSETS.getShader( "/engine/shaders/pbr/pbr_diffuse_irradiance.shader" );
            ASSERT( shader != ASSETS.getErrorShader() && "Please ensure that the given shader file exists." );

            auto diffuseIrradianceMat = RESOURCES.createMaterial( shader );
            diffuseIrradianceMat->setTexture( SHADER_NAME_ENV_MAP, cubemap );

            cmd.renderCubemap( m_diffuseIrradianceMap, diffuseIrradianceMat );
            Locator::getRenderer().dispatchImmediate( cmd );
        }

        // Specular reflection map
        {
            m_specularReflectionMap = RESOURCES.createCubemap();
            m_specularReflectionMap->create( specularReflectionSize, Graphics::TextureFormat::RGBAFloat, Graphics::Mips::Create );

            auto shader = ASSETS.getShader( "/engine/shaders/pbr/pbr_specular_reflection.shader" );
            ASSERT( shader != ASSETS.getErrorShader() && "Please ensure that the given shader file exists." );

            // Render into each mipmap
            Graphics::CommandBuffer cmd;
            U32 mipCount = m_specularReflectionMap->getMipCount();
            for (U32 mip = 0; mip < mipCount; mip++)
            {
                auto specularReflectionMaterial = RESOURCES.createMaterial( shader );
                specularReflectionMaterial->setTexture( SHADER_NAME_ENV_MAP, cubemap );
                specularReflectionMaterial->setFloat( SHADER_NAME_RESOLUTION, (F32)cubemap->getWidth() );

                F32 roughness = (F32)mip / (mipCount - 1);
                specularReflectionMaterial->setFloat( SHADER_NAME_ROUGHNESS, roughness );

                cmd.renderCubemap( m_specularReflectionMap, specularReflectionMaterial, mip );
            }
            Locator::getRenderer().dispatchImmediate( cmd );
        }
    }

} // End namespaces