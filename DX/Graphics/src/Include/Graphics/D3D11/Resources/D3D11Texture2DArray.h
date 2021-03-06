#pragma once
/**********************************************************************
    class: Texture2DArray (D3D11Texture.h)

    author: S. Hau
    date: April 3, 2018
**********************************************************************/

#include "i_texture2d_array.hpp"
#include "../D3D11.hpp"
#include "D3D11IBindableTexture.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class Texture2DArray : public Graphics::ITexture2DArray, public D3D11::IBindableTexture
    {
    public:
        Texture2DArray() = default;
        ~Texture2DArray() = default;

        //----------------------------------------------------------------------
        // ITexture2DArray Interface
        //----------------------------------------------------------------------
        void create(U32 width, U32 height, U32 depth, TextureFormat format, bool generateMips) override;
        void apply(bool updateMips, bool keepPixelsInRAM) override { IBindableTexture::apply(updateMips, keepPixelsInRAM); }

    private:
        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override { SAFE_RELEASE( m_pSampleState ); _CreateSampler( m_anisoLevel, m_filter, m_clampMode ); }
        void bind(const ShaderResourceDeclaration& res) override { IBindableTexture::bind(res); }
        U64* getNativeTexturePtr() const override { return reinterpret_cast<U64*>( m_pTexture ); }

        //----------------------------------------------------------------------
        void _CreateTextureArray(bool isDepthBuffer);
        void _CreateShaderResourveView(bool isDepthBuffer);

        //----------------------------------------------------------------------
        // IBindableTexture Interface
        //----------------------------------------------------------------------
        void _PushToGPU() override;

        //----------------------------------------------------------------------
        Texture2DArray(const Texture2DArray& other)               = delete;
        Texture2DArray& operator = (const Texture2DArray& other)  = delete;
        Texture2DArray(Texture2DArray&& other)                    = delete;
        Texture2DArray& operator = (Texture2DArray&& other)       = delete;
    };

} } // End namespaces