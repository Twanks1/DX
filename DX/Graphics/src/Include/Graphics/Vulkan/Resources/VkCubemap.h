#pragma once
/**********************************************************************
    class: Cubemap

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "i_cubemap.hpp"
#include "VkBindableTexture.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class Cubemap : public ICubemap, public IBindableTexture
    {
    public:
        Cubemap() = default;
        ~Cubemap() = default;

        //----------------------------------------------------------------------
        // ICubemap Interface
        //----------------------------------------------------------------------
        void create(I32 size, TextureFormat format, Mips mips) override;
        void apply(bool updateMips, bool keepPixelsInRAM) override { IBindableTexture::apply(updateMips, keepPixelsInRAM); }

    private:
        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override { IBindableTexture::_CreateSampler(m_anisoLevel, m_filter, m_clampMode); }
        void bind(const ShaderResourceDeclaration& res) override { IBindableTexture::bind(res); }
        U64* getNativeTexturePtr() const override { return reinterpret_cast<U64*>(m_image.img); }

        //----------------------------------------------------------------------
        // IBindableTexture Interface
        //----------------------------------------------------------------------
        void _PushToGPU() override;
        void _GenerateMips() override;

        //----------------------------------------------------------------------
        void _CreateTexture(bool isDepthBuffer);

        NULL_COPY_AND_ASSIGN(Cubemap)
    };

} } // End namespaces