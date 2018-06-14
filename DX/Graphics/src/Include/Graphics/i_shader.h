#pragma once
/**********************************************************************
    class: IShader (i_shader.h)

    author: S. Hau
    date: March 12, 2018

    Interface for a Shader class. A shader can be applied to a material.
    A shader represents a full pipeline from vertex-shader up to the
    fragment-shader, with all the state settings required to render it.
**********************************************************************/

#include "OS/FileSystem/path.h"
#include "structs.hpp"
#include "vertex_layout.hpp"
#include "i_texture.h"
#include "shader_resources.hpp"
#include <functional>

namespace Graphics {

    typedef std::function<void(IShader*)> ShaderReloadCallback;

    //**********************************************************************
    // Shaders with a renderqueue >= BackToFrontBoundary will be rendered back to front.
    enum class RenderQueue : I32
    {
        // --------- FRONT TO BACK ------------
        Background          = 0,
        Geometry            = 1000,
        AlphaTest           = 2000,
        // --------- BACK TO FRONT -------------
        BackToFrontBoundary = 3000,
        Transparent         = 4000,
        Overlay             = 5000
    };

    //**********************************************************************
    class IShader
    {
    public:
        IShader() = default;
        virtual ~IShader() {}

        //----------------------------------------------------------------------
        const String&                   getName()           const { return m_name; }
        I32                             getRenderQueue()    const { return m_renderQueue; }

        void setName(const String& name) { m_name = name; }

        //----------------------------------------------------------------------
        // @Return:
        //  True, if the corresponding shader exists.
        //----------------------------------------------------------------------
        virtual bool hasFragmentShader() const = 0;
        virtual bool hasGeometryShader() const = 0;
        virtual bool hasTessellationShader() const = 0;

        //----------------------------------------------------------------------
        // Try to compile this shader.
        // @Params:
        //  "vertPath": Path to the vertex shader file.
        //  "fragPath": Path to the fragment shader file.
        //  "entryPoint": Entry point of the shader.
        // @Return:
        //  True, if compilation was successful, otherwise false and prints what went wrong.
        //----------------------------------------------------------------------
        virtual bool compileFromFile(const OS::Path& vertPath, const OS::Path& fragPath, CString entryPoint) = 0;

        //----------------------------------------------------------------------
        // Try to compile this shader.
        // @Params:
        //  "vertSrc": Source of the vertex shader in ASCII.
        //  "fragSrc": Source of the fragment shader in ASCII.
        //  "entryPoint": Entry point of the shader.
        // @Return:
        //  True, if compilation was successful, otherwise false and prints what went wrong.
        //----------------------------------------------------------------------
        virtual bool compileFromSource(const String& vertSrc, const String& fragSrc, CString entryPoint) = 0;

        //----------------------------------------------------------------------
        // Try to compile one shader.
        // @Params:
        //  "src": Source of the shader in ASCII.
        //  "entryPoint": Entry point of the shader.
        // @Return:
        //  True, if compilation was successful, otherwise false and prints what went wrong.
        //----------------------------------------------------------------------
        virtual bool compileVertexShaderFromSource(const String& src, CString entryPoint) = 0;
        virtual bool compileFragmentShaderFromSource(const String& src, CString entryPoint) = 0;
        virtual bool compileGeometryShaderFromSource(const String& src, CString entryPoint) = 0;
        //virtual bool compileTessellationShaderFromSource(const String& src, CString entryPoint) = 0;

        //----------------------------------------------------------------------
        // Recompile all shaders which are not up to date. Does nothing if shader was compiled from source.
        // @Return:
        //  List of shader paths, which were recompiled.
        //----------------------------------------------------------------------
        virtual ArrayList<OS::Path> recompile() = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  True, if all shader files are up-to-date (weren't modified since they were compiled).
        //----------------------------------------------------------------------
        virtual bool isUpToDate() = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  All shaderpaths used by this shader class.
        //----------------------------------------------------------------------
        virtual ArrayList<OS::Path> getShaderPaths() const = 0;

        //----------------------------------------------------------------------
        // Change pipeline states for rendering this shader
        //----------------------------------------------------------------------
        virtual void setRasterizationState(const RasterizationState& rzState ) = 0;
        virtual void setDepthStencilState(const DepthStencilState& dsState ) = 0;
        virtual void setBlendState(const BlendState& bState ) = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  The vertex layout from the attached vertex-shader.
        //----------------------------------------------------------------------
        virtual const VertexLayout& getVertexLayout() const = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  Information about the corresponding uniform buffer which is used by a material.
        //----------------------------------------------------------------------
        virtual const ShaderUniformBufferDeclaration* getVSUniformMaterialBuffer() const = 0;
        virtual const ShaderUniformBufferDeclaration* getFSUniformMaterialBuffer() const = 0;
        virtual const ShaderUniformBufferDeclaration* getGSUniformMaterialBuffer() const = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  Information about the corresponding uniform buffer which is used by a shader.
        //----------------------------------------------------------------------
        virtual const ShaderUniformBufferDeclaration* getVSUniformShaderBuffer() const = 0;
        virtual const ShaderUniformBufferDeclaration* getFSUniformShaderBuffer() const = 0;
        virtual const ShaderUniformBufferDeclaration* getGSUniformShaderBuffer() const = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  The resource declaration with name 'name' across all shader stages. Nullptr if not existent.
        //----------------------------------------------------------------------
        virtual const ShaderResourceDeclaration* getShaderResource(StringID name) const = 0;

        //----------------------------------------------------------------------
        // @Return:
        //  The datatype of a property with the given name. Issues a warning if the
        //  name exists in more than one shader stage. (This does not include shader resources like textures)
        //----------------------------------------------------------------------
        DataType getDataTypeOfMaterialProperty(StringID name);
        DataType getDataTypeOfMaterialPropertyOrResource(StringID name);

        //----------------------------------------------------------------------
        // @Return:
        //  True if the property exists and is considered to be part of the material. (This does not include shader resources like textures)
        //----------------------------------------------------------------------
        bool hasMaterialProperty(StringID name){ return getDataTypeOfMaterialProperty(name) != DataType::Unknown; }

        //----------------------------------------------------------------------
        // @Return:
        //  The datatype of a property with the given name. Issues a warning if the
        //  name exists in more than one shader stage. (This does not include shader resources like textures)
        //----------------------------------------------------------------------
        DataType getDataTypeOfShaderProperty(StringID name);

        //----------------------------------------------------------------------
        // @Return:
        //  True if the property exists and is considered to be part of the shader. (This does not include shader resources like textures)
        //----------------------------------------------------------------------
        bool hasShaderProperty(StringID name) { return getDataTypeOfShaderProperty(name) != DataType::Unknown; }

        //----------------------------------------------------------------------
        // @Return:
        //  Manual Blend-Factors. Only used when manual blending is enabled.
        //----------------------------------------------------------------------
        std::array<F32, 4> getBlendFactors() const { return m_blendFactors; }

        //----------------------------------------------------------------------
        // Set manually the blend factors. Only used when manual blending is enabled.
        //----------------------------------------------------------------------
        void setBlendFactors(std::array<F32, 4> newBlendFactors) { m_blendFactors = newBlendFactors; }

        //----------------------------------------------------------------------
        // Convenvience function for enabling the standard alpha blending
        //----------------------------------------------------------------------
        void enableAlphaBlending(bool alphaToCoverage = false)
        {
            Graphics::BlendState blendState;
            blendState.alphaToCoverage = alphaToCoverage;
            blendState.blendStates[0].blendEnable    = true;
            blendState.blendStates[0].srcBlend       = Graphics::Blend::SrcAlpha;
            blendState.blendStates[0].destBlend      = Graphics::Blend::InvSrcAlpha;
            blendState.blendStates[0].blendOp        = Graphics::BlendOP::Add;
            blendState.blendStates[0].srcBlendAlpha  = Graphics::Blend::One;
            blendState.blendStates[0].destBlendAlpha = Graphics::Blend::Zero;
            blendState.blendStates[0].blendOpAlpha   = Graphics::BlendOP::Add;

            setBlendState( blendState );
        }

        //----------------------------------------------------------------------
        // Set the renderqueue for this shader.
        //----------------------------------------------------------------------
        void setRenderQueue(I32 renderQueue) { m_renderQueue = renderQueue; }

        //----------------------------------------------------------------------
        // The given function will be invoked whenever the shader gets reloaded.
        //----------------------------------------------------------------------
        void setReloadCallback(const ShaderReloadCallback& f) { m_reloadCallback = f; }

        //----------------------------------------------------------------------
        // Invokes the reload callback if one exists.
        //----------------------------------------------------------------------
        void invokeReloadCallback() { if (m_reloadCallback) m_reloadCallback(this); }

        //**********************************************************************
        // Shader Buffer
        //**********************************************************************
        //----------------------------------------------------------------------
        I32                 getInt(StringID name)       const;
        F32                 getFloat(StringID name)     const;
        Math::Vec4          getVec4(StringID name)      const;
        DirectX::XMMATRIX   getMatrix(StringID name)    const;
        Color               getColor(StringID name)     const;
        TexturePtr          getTexture(StringID name)   const;

        //----------------------------------------------------------------------
        I32                 getInt(CString name)       const { return getInt(SID(name)); }
        F32                 getFloat(CString name)     const { return getFloat(SID(name)); }
        Math::Vec4          getVec4(CString name)      const { return getVec4(SID(name)); }
        DirectX::XMMATRIX   getMatrix(CString name)    const { return getMatrix(SID(name)); }
        Color               getColor(CString name)     const { return getColor(SID(name)); }
        TexturePtr          getTexture(CString name)   const { return getTexture(SID(name)); }

        //----------------------------------------------------------------------
        void setInt(StringID name, I32 val);
        void setFloat(StringID name, F32 val);
        void setVec4(StringID name, const Math::Vec4& vec);
        void setMatrix(StringID name, const DirectX::XMMATRIX& matrix);
        void setColor(StringID name, Color color);
        void setTexture(StringID name, const TexturePtr& tex);

        inline void setInt(CString name, I32 val)                           { setInt(SID(name), val); }
        inline void setFloat(CString name, F32 val)                         { setFloat(SID(name), val); }
        inline void setVec4(CString name, const Math::Vec4& vec)            { setVec4(SID(name), vec); }
        inline void setMatrix(CString name, const DirectX::XMMATRIX& matrix){ setMatrix(SID(name), matrix); }
        inline void setColor(CString name, Color color)                     { setColor(SID(name), color); }
        inline void setTexture(CString name, const TexturePtr& tex)         { setTexture(SID(name), tex); }


    protected:
        std::array<F32, 4>  m_blendFactors  = { 1.0f, 1.0f, 1.0f, 1.0f };   // These are only used when blending is enabled
        String              m_name          = "NO NAME";
        I32                 m_renderQueue   = (I32)RenderQueue::Geometry;

        // Data maps
        HashMap<StringID, I32>                  m_intMap;
        HashMap<StringID, F32>                  m_floatMap;
        HashMap<StringID, Math::Vec4>           m_vec4Map;
        HashMap<StringID, DirectX::XMMATRIX>    m_matrixMap;
        HashMap<StringID, TexturePtr>           m_textureMap;

        ShaderReloadCallback                    m_reloadCallback = nullptr;

        // Bind all textures in the texture map
        void _BindTextures();

        virtual void _SetInt(StringID name, I32 val) = 0;
        virtual void _SetFloat(StringID name, F32 val) = 0;
        virtual void _SetVec4(StringID name, const Math::Vec4& vec) = 0;
        virtual void _SetMatrix(StringID name, const DirectX::XMMATRIX& matrix) = 0;

        void _ClearAllMaps();

    private:
        //----------------------------------------------------------------------
        friend class D3D11Renderer;
        virtual void bind() = 0;
        virtual void unbind() = 0;

        //----------------------------------------------------------------------
        IShader(const IShader& other)               = delete;
        IShader& operator = (const IShader& other)  = delete;
        IShader(IShader&& other)                    = delete;
        IShader& operator = (IShader&& other)       = delete;
    };

    using Shader = IShader;

} // End namespaces

using ShaderPtr = std::shared_ptr<Graphics::IShader>;