#include "D3D11RenderTexture.h"
/**********************************************************************
    class: RenderTexture (D3D11RenderTexture.cpp)

    author: S. Hau
    date: March 30, 2018
**********************************************************************/

#include "../D3D11Utility.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    RenderTexture::RenderTexture( U32 width, U32 height, U32 depth, TextureFormat format )
        : IRenderTexture( width, height, depth, format )
    {
        for (I32 i = 0; i < NUM_BUFFERS; i++)
        {
            _CreateRenderTargetAndView(i);
            if (m_depth > 0)
                _CreateDepthBuffer(i);
        }
        _CreateSampler();
    }

    //----------------------------------------------------------------------
    RenderTexture::~RenderTexture()
    {
        for (I32 i = 0; i < NUM_BUFFERS; i++)
        {
            SAFE_RELEASE( m_buffers[i].m_pRenderTexture );
            SAFE_RELEASE( m_buffers[i].m_pRenderTextureView );
            SAFE_RELEASE( m_buffers[i].m_pRenderTargetView );
            SAFE_RELEASE( m_buffers[i].m_pDepthStencilBuffer );
            SAFE_RELEASE( m_buffers[i].m_pDepthStencilView );
        }
        SAFE_RELEASE( m_pSampleState );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderTexture::bind( U32 slot )
    {
        g_pImmediateContext->PSSetSamplers( slot, 1, &m_pSampleState );

        // Bind previous rendered framebuffer
        I32 index = (m_index - 1);
        if (index < 0)
            index += NUM_BUFFERS;
        g_pImmediateContext->PSSetShaderResources( slot, 1, &m_buffers[index].m_pRenderTextureView );
    }

    //----------------------------------------------------------------------
    void RenderTexture::bindForRendering()
    {
        m_index = (m_index + 1) % NUM_BUFFERS;
        g_pImmediateContext->OMSetRenderTargets( 1, &m_buffers[m_index].m_pRenderTargetView, m_buffers[m_index].m_pDepthStencilView );
    }

    //----------------------------------------------------------------------
    void RenderTexture::clear( Color color, F32 depth, U8 stencil )
    {
        g_pImmediateContext->ClearRenderTargetView( m_buffers[m_index].m_pRenderTargetView, color.normalized().data() );
        g_pImmediateContext->ClearDepthStencilView( m_buffers[m_index].m_pDepthStencilView, (D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL), 1.0f, 0 );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void RenderTexture::_CreateRenderTargetAndView( I32 index )
    {   
        // Setup the description of the texture
        D3D11_TEXTURE2D_DESC textureDesc;
        textureDesc.Height              = getHeight();
        textureDesc.Width               = getWidth();
        textureDesc.MipLevels           = 1;
        textureDesc.ArraySize           = 1;
        textureDesc.Format              = Utility::TranslateTextureFormat( m_format );
        textureDesc.SampleDesc.Count    = 1;
        textureDesc.SampleDesc.Quality  = 0;
        textureDesc.Usage               = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        textureDesc.CPUAccessFlags      = 0;
        textureDesc.MiscFlags           = 0;

        // Create texture
        HR( g_pDevice->CreateTexture2D( &textureDesc, NULL, &m_buffers[index].m_pRenderTexture) );

        // Setup the shader resource view description
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = -1;

        HR( g_pDevice->CreateShaderResourceView( m_buffers[index].m_pRenderTexture, &srvDesc, &m_buffers[index].m_pRenderTextureView ) );

        HR( g_pDevice->CreateRenderTargetView( m_buffers[index].m_pRenderTexture, NULL, &m_buffers[index].m_pRenderTargetView ) );
    }

    //----------------------------------------------------------------------
    void RenderTexture::_CreateDepthBuffer( I32 index )
    {
        D3D11_TEXTURE2D_DESC depthStencilDesc = {};
        depthStencilDesc.Width          = m_width;
        depthStencilDesc.Height         = m_height;
        depthStencilDesc.MipLevels      = 1;
        depthStencilDesc.ArraySize      = 1;
        depthStencilDesc.Format         = Utility::TranslateDepthFormat( m_depth );
        depthStencilDesc.SampleDesc     = { 1, 0 };
        depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags      = 0;

        HR( g_pDevice->CreateTexture2D( &depthStencilDesc, NULL, &m_buffers[index].m_pDepthStencilBuffer) );
        HR( g_pDevice->CreateDepthStencilView( m_buffers[index].m_pDepthStencilBuffer, NULL, &m_buffers[index].m_pDepthStencilView ) );
    }

    //----------------------------------------------------------------------
    void RenderTexture::_CreateSampler()
    {
        SAFE_RELEASE( m_pSampleState );

        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = (m_anisoLevel > 1 ? D3D11_FILTER_ANISOTROPIC : Utility::TranslateFilter( m_filter ) );

        auto clampMode = Utility::TranslateClampMode( m_clampMode );
        samplerDesc.AddressU        = clampMode;
        samplerDesc.AddressV        = clampMode;
        samplerDesc.AddressW        = clampMode;
        samplerDesc.MipLODBias      = 0.0f;
        samplerDesc.MaxAnisotropy   = m_anisoLevel;
        samplerDesc.ComparisonFunc  = D3D11_COMPARISON_NEVER;
        samplerDesc.BorderColor[0]  = 0.0f;
        samplerDesc.BorderColor[1]  = 0.0f;
        samplerDesc.BorderColor[2]  = 0.0f;
        samplerDesc.BorderColor[3]  = 0.0f;
        samplerDesc.MinLOD          = 0;
        samplerDesc.MaxLOD          = D3D11_FLOAT32_MAX;

        HR( g_pDevice->CreateSamplerState( &samplerDesc, &m_pSampleState ) );
    }

} } // End namespaces