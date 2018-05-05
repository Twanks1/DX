#include "asset_manager.h"
/**********************************************************************
    class: AssetManager (asset_manager.cpp)

    author: S. Hau
    date: April 9, 2018
**********************************************************************/

#include "Ext/StbImage/stb_image.h"
#include "Core/locator.h"
#include "shader_parser.hpp"
#include "material_parser.hpp"

namespace Core { namespace Assets {

    #define HOT_RELOAD_INTERVAL_MILLIS  500
    #define LOG_COLOR                   Color::GREEN
    
    //----------------------------------------------------------------------
    void AssetManager::init()
    {
    }

    //----------------------------------------------------------------------
    void AssetManager::shutdown()
    {
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    Texture2DPtr AssetManager::getTexture2D( const OS::Path& filePath, bool generateMips )
    {
        // Check if texture was already loaded
        StringID pathAsID = SID( StringUtils::toLower( filePath.toString() ).c_str() );
        if ( m_textureCache.find( pathAsID ) != m_textureCache.end() )
        {
            auto weakPtr = m_textureCache[pathAsID].texture;
            if ( not weakPtr.expired() )
                return Texture2DPtr( weakPtr );
        }

        // Try loading texture
        LOG( "AssetManager: Loading Texture '" + filePath.toString() + "'", LOG_COLOR );
        auto texture = _LoadTexture2D( filePath, generateMips );
        if ( not texture )
        {
            LOG_WARN( "LoadTexture(): Texture '" + filePath.toString() + "' could not be loaded. Returning the default texture instead." );
            return RESOURCES.getWhiteTexture();
        }

        TextureAssetInfo texInfo;
        texInfo.texture     = texture;
        texInfo.path        = filePath;
        texInfo.timeAtLoad  = filePath.getLastWrittenFileTime();

        m_textureCache[pathAsID] = texInfo;

        return texture;
    }

    //----------------------------------------------------------------------
    void AssetManager::getTexture2DAsync( const OS::Path& filePath, bool genMips, const std::function<void(Texture2DPtr)>& callback )
    {
        ASYNC_JOB([=] {
            auto tex = getTexture2D( filePath, genMips );
            callback( tex );
        });
    }

    //----------------------------------------------------------------------
    CubemapPtr AssetManager::getCubemap( const OS::Path& posX, const OS::Path& negX,
                                         const OS::Path& posY, const OS::Path& negY,
                                         const OS::Path& posZ, const OS::Path& negZ, bool generateMips )
    {
        // Check if cubemap was already loaded (checks only first path)
        StringID pathAsID = SID( StringUtils::toLower( posX.toString() ).c_str() );
        if ( m_cubemapCache.find( pathAsID ) != m_cubemapCache.end() )
        {
            auto weakPtr = m_cubemapCache[pathAsID].cubemap;
            if ( not weakPtr.expired() )
                return CubemapPtr( weakPtr );
        }

        // Try loading cubemap
        LOG( "AssetManager: Loading 6 Cubemap Faces '" + posX.toString() + "' (Positive X-Face) etc.", LOG_COLOR );
        auto cubemap = _LoadCubemap( posX, negX, posY, negY, posZ, negZ, generateMips );
        if (not cubemap)
        {
            LOG_WARN( "LoadCubemap(): At least one of the specified cubemap faces couldn't be loaded! Positive X-Face path was " +
                   posX.toString() + " .Returning default cubemap instead." );
            return RESOURCES.getDefaultCubemap();
        }

        CubemapAssetInfo texInfo;
        texInfo.cubemap     = cubemap;
        texInfo.path        = posX;
        texInfo.timeAtLoad  = posX.getLastWrittenFileTime();

        m_cubemapCache[pathAsID] = texInfo;

        return cubemap;
    }

    //----------------------------------------------------------------------
    AudioClipPtr AssetManager::getAudioClip( const OS::Path& filePath )
    {
        // Check if audio was already loaded
        StringID pathAsID = SID( StringUtils::toLower( filePath.toString() ).c_str() );
        if ( m_audioCache.find( pathAsID ) != m_audioCache.end() )
        {
            auto weakPtr = m_audioCache[pathAsID].wavClip;
            if ( not weakPtr.expired() )
            {
                auto audioClip = RESOURCES.createAudioClip();
                audioClip->setWAVClip( Audio::WAVClipPtr( weakPtr ) );
                return audioClip;
            }
        }

        // Try loading audio
        LOG( "AssetManager: Loading Audio '" + filePath.toString() + "'", LOG_COLOR );

        auto wav = std::make_shared<Audio::WAVClip>();
        if( not wav->load( filePath ) )
        {
            LOG_WARN( "AssetManager::getAudioClip(): Audio clip '" + filePath.toString() + "' could not be loaded. Returning nullptr." );
            return nullptr;
        }

        auto audioClip = RESOURCES.createAudioClip();
        audioClip->setWAVClip( wav );

        // Cache loaded audio
        AudioClipAssetInfo info;
        info.wavClip    = wav;
        info.path       = filePath;
        info.timeAtLoad = filePath.getLastWrittenFileTime();

        m_audioCache[pathAsID] = info;

        return audioClip;
    }

    //----------------------------------------------------------------------
    ShaderPtr AssetManager::getShader( const OS::Path& filePath )
    {
        // Check if shader was already loaded
        StringID pathAsID = SID( StringUtils::toLower( filePath.toString() ).c_str() );
        if ( m_shaderCache.find( pathAsID ) != m_shaderCache.end() )
        {
            auto weakPtr = m_shaderCache[pathAsID].shader;
            if ( not weakPtr.expired() )
                return ShaderPtr( weakPtr );
        }

        // Try loading shader
        LOG( "AssetManager: Loading Shader '" + filePath.toString() + "'", LOG_COLOR );
        try 
        {
            auto shader = ShaderParser::LoadShader( filePath );

            ShaderAssetInfo shaderInfo;
            shaderInfo.shader      = shader;
            shaderInfo.path        = filePath;
            shaderInfo.timeAtLoad  = filePath.getLastWrittenFileTime();

            m_shaderCache[pathAsID] = shaderInfo;

            return shader;
        }
        catch (const std::runtime_error& e) 
        {
            LOG_WARN( "AssetManager::getShader(): Shader '" + filePath.toString() + "' could not be loaded. Reason: " 
                      + e.what() +  " Returning the error shader instead." );
            return RESOURCES.getErrorShader();
        }
    }

    //----------------------------------------------------------------------
    MaterialPtr AssetManager::getMaterial( const OS::Path& filePath )
    {
        // Check if material was already loaded
        StringID pathAsID = SID( StringUtils::toLower( filePath.toString() ).c_str() );
        if ( m_materialCache.find( pathAsID ) != m_materialCache.end() )
        {
            auto weakPtr = m_materialCache[pathAsID].material;
            if ( not weakPtr.expired() )
                return MaterialPtr( weakPtr );
        }

        // Try loading shader
        LOG( "AssetManager: Loading Material '" + filePath.toString() + "'", LOG_COLOR );
        try 
        {
            MaterialPtr material = MaterialParser::LoadMaterial( filePath );

            MaterialAssetInfo materialInfo;
            materialInfo.material    = material;
            materialInfo.path        = filePath;
            materialInfo.timeAtLoad  = filePath.getLastWrittenFileTime();

            m_materialCache[pathAsID] = materialInfo;

            return material;
        }
        catch ( const std::runtime_error& e )
        {
            LOG_WARN( "AssetManager::getMaterial(): Material '" + filePath.toString() + "' could not be loaded. Reason: " 
                      + e.what() +  " Returning the default material instead." );
            return RESOURCES.getDefaultMaterial();
        }
    }

    //----------------------------------------------------------------------
    void AssetManager::setHotReloading( bool enabled ) 
    { 
        m_hotReloading = enabled;
        if (m_hotReloading)
            _EnableHotReloading();
        else
            Locator::getEngineClock().clearCallback( m_hotReloadingCallback );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    Texture2DPtr AssetManager::_LoadTexture2D( const OS::Path& filePath, bool generateMips )
    {
        I32 width, height, bpp;
        auto pixels = stbi_load( filePath.c_str(), &width, &height, &bpp, 4 );

        if (pixels)
        {
            auto tex = RESOURCES.createTexture2D( width, height, Graphics::TextureFormat::RGBA32, generateMips );
            tex->setPixels( pixels );
            tex->apply();

            stbi_image_free( pixels );

            return tex;
        }

        return nullptr;
    }

    //----------------------------------------------------------------------
    CubemapPtr AssetManager::_LoadCubemap( const OS::Path& posX, const OS::Path& negX,
                                           const OS::Path& posY, const OS::Path& negY,
                                           const OS::Path& posZ, const OS::Path& negZ, bool generateMips )
    {
        I32 width, height, bpp;
        auto posXPixels = stbi_load( posX.c_str(), &width, &height, &bpp, 4 );
        auto negXPixels = stbi_load( negX.c_str(), &width, &height, &bpp, 4 );
        auto posYPixels = stbi_load( posY.c_str(), &width, &height, &bpp, 4 );
        auto negYPixels = stbi_load( negY.c_str(), &width, &height, &bpp, 4 );
        auto posZPixels = stbi_load( posZ.c_str(), &width, &height, &bpp, 4 );
        auto negZPixels = stbi_load( negZ.c_str(), &width, &height, &bpp, 4 );

        if ( not posXPixels || not negXPixels || not posYPixels || not negYPixels || not posZPixels || not negZPixels )
        {
            stbi_image_free( posXPixels );
            stbi_image_free( negXPixels );
            stbi_image_free( posYPixels );
            stbi_image_free( negYPixels );
            stbi_image_free( posZPixels );
            stbi_image_free( negZPixels );
            return nullptr;
        }

        auto cubemap = RESOURCES.createCubemap();
        cubemap->create( width, Graphics::TextureFormat::RGBA32, generateMips );
      
        cubemap->setPixels( Graphics::CubemapFace::PositiveX, posXPixels );
        cubemap->setPixels( Graphics::CubemapFace::NegativeX, negXPixels );
        cubemap->setPixels( Graphics::CubemapFace::PositiveY, posYPixels );
        cubemap->setPixels( Graphics::CubemapFace::NegativeY, negYPixels );
        cubemap->setPixels( Graphics::CubemapFace::PositiveZ, posZPixels );
        cubemap->setPixels( Graphics::CubemapFace::NegativeZ, negZPixels );

        stbi_image_free( posXPixels );
        stbi_image_free( negXPixels );
        stbi_image_free( posYPixels );
        stbi_image_free( negYPixels );
        stbi_image_free( posZPixels );
        stbi_image_free( negZPixels );

        cubemap->apply();
        return cubemap;
    }

    //----------------------------------------------------------------------
    void AssetManager::_EnableHotReloading()
    {
        // HOT-RELOADING CALLBACK
        m_hotReloadingCallback = Locator::getEngineClock().setInterval([this]{

            // Texture reloading
            for ( auto it = m_textureCache.begin(); it != m_textureCache.end(); )
            {
                if ( it->second.texture.expired() )
                {
                    // Texture does no longer exist, so remove it from the cache map
                    it = m_textureCache.erase( it );
                }
                else
                {
                    it->second.ReloadIfNotUpToDate();
                    it++;
                }
            }

            // Shader reloading
            for ( auto it = m_shaderCache.begin(); it != m_shaderCache.end(); )
            {
                if ( it->second.shader.expired() )
                {
                    // Shader does no longer exist, so remove it from the cache map
                    it = m_shaderCache.erase( it );
                }
                else
                {
                    it->second.ReloadIfNotUpToDate( *this );
                    it++;
                }
            }

            // Material reloading
            for ( auto it = m_materialCache.begin(); it != m_materialCache.end(); )
            {
                if ( it->second.material.expired() )
                {
                    // Material does no longer exist, so remove it from the cache map
                    it = m_materialCache.erase( it );
                }
                else
                {
                    it->second.ReloadIfNotUpToDate();
                    it++;
                }
            }

        }, HOT_RELOAD_INTERVAL_MILLIS);
    }

    //**********************************************************************
    // PRIVATE - ASSET INFOS
    //**********************************************************************

    //----------------------------------------------------------------------
    void AssetManager::TextureAssetInfo::ReloadIfNotUpToDate()
    {
        if ( auto tex = texture.lock() )
        {
            try {
                auto currentFileTime = path.getLastWrittenFileTime();

                if (timeAtLoad != currentFileTime)
                {
                    // Reload texture on a separate thread
                    LOG( "Reloading texture: " + path.toString(), LOG_COLOR );
                    ASYNC_JOB([=] {
                        I32 width, height, bpp;
                        auto pixels = stbi_load( path.c_str(), &width, &height, &bpp, 4 );
                        tex->setPixels( pixels );
                        tex->apply();
                        stbi_image_free( pixels );
                    });

                    timeAtLoad = currentFileTime;
                }
            }
            catch (...) {
                // Do nothing here. This means simply the file could not be opened, because another app has not yet closed the handle
            }
        }
    }

    //----------------------------------------------------------------------
    void AssetManager::ShaderAssetInfo::ReloadIfNotUpToDate(const AssetManager& sm)
    {
        if ( auto sh = shader.lock() )
        {
            try {
                auto currentFileTime = path.getLastWrittenFileTime();

                if (timeAtLoad != currentFileTime)
                {
                    LOG( "Reloading shader: " + path.toString(), LOG_COLOR );
                    try {
                        ShaderParser::UpdateShader( sh, path );

                        // Reload every material which has this shader applied
                        for (auto& pair : sm.m_materialCache)
                            if ( auto mat = pair.second.material.lock() )
                                if (mat->getShader() == sh)
                                {
                                    LOG( "Reloading material: " + path.toString(), LOG_COLOR );
                                    MaterialParser::UpdateMaterial( mat, pair.second.path, true );
                                }

                    } catch(const std::runtime_error& e) { 
                        LOG_WARN( String( "Failed to reload shader. Reason: " ) + e.what() );
                    }

                    timeAtLoad = currentFileTime;
                }
            }
            catch (...) {
                // Do nothing here. This means simply the file could not be opened, because another app has not yet closed the handle
            }
        }
    }

    //----------------------------------------------------------------------
    void AssetManager::MaterialAssetInfo::ReloadIfNotUpToDate()
    {
        if ( auto mat = material.lock() )
        {
            try {
                auto currentFileTime = path.getLastWrittenFileTime();

                if (timeAtLoad != currentFileTime)
                {
                    LOG( "Reloading material: " + path.toString(), LOG_COLOR );
                    ASYNC_JOB([=] {
                        try {
                            MaterialParser::UpdateMaterial( mat, path );
                        }
                        catch (const std::runtime_error& e) {
                            LOG_WARN( String( "Failed to reload material. Reason: " ) + e.what() );
                        }
                    });

                    timeAtLoad = currentFileTime;
                }
            }
            catch (...) {
                // Do nothing here. This means simply the file could not be opened, because another app has not yet closed the handle
            }
        }
    }

} } // End namespaces