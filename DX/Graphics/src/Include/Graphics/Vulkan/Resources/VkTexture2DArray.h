#pragma once
/**********************************************************************
    class: Texture2DArray

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "i_texture2d_array.hpp"
#include "VkBindableTexture.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class Texture2DArray : public ITexture2DArray, public IBindableTexture
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
        void _UpdateSampler() override { IBindableTexture::_RecreateSampler(m_anisoLevel, m_filter, m_clampMode); }
        void bind(const ShaderResourceDeclaration& res) override { IBindableTexture::bind(res); }

        //----------------------------------------------------------------------
        // IBindableTexture Interface
        //----------------------------------------------------------------------
        void _PushToGPU();

        //----------------------------------------------------------------------
        void _CreateTextureArray();

        NULL_COPY_AND_ASSIGN(Texture2DArray)
    };

} } // End namespaces