#pragma once
/**********************************************************************
    class: RenderBuffer (render_buffer.hpp)

    author: S. Hau
    date: June 6, 2018

    Texture on the gpu, in which can be rendered (e.g. color or depth)
**********************************************************************/

#include "i_texture.h"
#include "structs.hpp"

namespace Graphics
{

    //**********************************************************************
    class IRenderBuffer : public ITexture
    {
    public:
        IRenderBuffer() : ITexture(TextureDimension::Tex2D) {}
        virtual ~IRenderBuffer() = default;

        //----------------------------------------------------------------------
        const MSAASamples getSampleCount() const   { return m_sampleCount; }
        bool              isDepthBuffer()  const   { return m_format == TextureFormat::D32 || m_format == TextureFormat::D24S8 || m_format == TextureFormat::D16; }
        bool              isColorBuffer()  const   { return not isDepthBuffer(); }
        bool              isMultisampled() const   { return (I32)m_sampleCount > 1;}

        //----------------------------------------------------------------------
        // Creates a new color render buffer.
        // @Params:
        //  "width": Width in pixels.
        //  "height": Height in pixels.
        //  "format": The texture format.
        //  "samplingDesc": Whether the buffer will be multisampled or not.
        //----------------------------------------------------------------------
        virtual void create(U32 width, U32 height, TextureFormat format, MSAASamples samples = MSAASamples::One) = 0;

        //----------------------------------------------------------------------
        // Recreate all buffers in this render texture.
        //----------------------------------------------------------------------
        virtual void recreate(U32 w, U32 h) = 0;
        virtual void recreate(U32 w, U32 h, MSAASamples samples) = 0;
        virtual void recreate(Graphics::TextureFormat format) = 0;

    protected:
        MSAASamples m_sampleCount;

        //----------------------------------------------------------------------
        // Binds this renderbuffer to the output merger.
        // P.S. Using this binds only a color OR depth texture but never both. For this use a render-texture.
        //----------------------------------------------------------------------
        friend class D3D11Renderer;
        friend class VkRenderer;
        virtual void bindForRendering() = 0;

        //----------------------------------------------------------------------
        // Clears the buffer with the given color data.
        //----------------------------------------------------------------------
        friend class IRenderTexture;
        virtual void clearColor(Color color) = 0;

        //----------------------------------------------------------------------
        // Clears the buffer with the given depth and stencil data.
        //----------------------------------------------------------------------
        virtual void clearDepthStencil(F32 depth, U8 stencil) = 0;

        NULL_COPY_AND_ASSIGN(IRenderBuffer)
    };

    using RenderBuffer = IRenderBuffer;

}

using RenderBufferPtr = std::shared_ptr<Graphics::IRenderBuffer>;