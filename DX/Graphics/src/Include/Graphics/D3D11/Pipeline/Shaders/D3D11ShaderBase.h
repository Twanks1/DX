#pragma once
/**********************************************************************
    class: ShaderBase (D3D11ShaderBase.h)

    author: S. Hau
    date: March 17, 2018
**********************************************************************/

#include "../../D3D11.hpp"
#include "OS/FileSystem/path.h"
#include <d3dcompiler.h>

namespace Graphics { namespace D3D11 {

    // Contains infos about one specific member in a shader buffer
    struct ConstantBufferMemberInfo
    {
        StringID    name;
        U32         offset = 0;
        Size        size = 0;
    };

    // Contains information about an buffer in a shader
    struct ConstantBufferInfo
    {
        StringID                            name;
        U32                                 slot = 0;
        Size                                sizeInBytes = 0;
        ArrayList<ConstantBufferMemberInfo> members;
    };

    // Contains information about bound resources
    struct TextureBindingInfo
    {
        StringID    name;
        U32         slot;
    };

    //**********************************************************************
    class ShaderBase
    {
    public:
        ShaderBase() = default;
        virtual ~ShaderBase() { SAFE_RELEASE( m_pShaderBlob ); SAFE_RELEASE( m_pShaderReflection ); }

        //----------------------------------------------------------------------
        virtual void bind() = 0;
        virtual bool compileFromFile(const OS::Path& path, CString entryPoint) = 0;
        virtual bool compileFromSource(const String& shaderSource, CString entryPoint) = 0;

        //----------------------------------------------------------------------
        const OS::Path& getFilePath()   const { return m_filePath; }
        CString         getEntryPoint() const { return m_entryPoint.c_str(); }
        ID3DBlob*       getShaderBlob()       { return m_pShaderBlob; }
        bool            recompile();

        //----------------------------------------------------------------------
        // @Return:
        //  Whether the shader (if compiled from file) is up to date on disk.
        //  This returns always true if the shader was compiled from source.
        //----------------------------------------------------------------------
        bool isUpToDate() const;

        //----------------------------------------------------------------------
        // @Return:
        //  List of information about every constant buffer found via reflection.
        //----------------------------------------------------------------------
        const ArrayList<ConstantBufferInfo>&    getConstantBufferInfos()  const { return m_constantBufferInfos; }

        //----------------------------------------------------------------------
        // @Return:
        //  Constant buffer info about first constant buffer with name "material" in it.
        // Note: Use "hasMaterialBuffer()" before calling this function to ensure 
        // that this shader has an material constant buffer!
        //----------------------------------------------------------------------
        const ConstantBufferInfo&               getMaterialBufferInfo()  const;

        //----------------------------------------------------------------------
        // @Return:
        //  True, when a material constant buffer exists in this shader.
        //  (A buffer with name 'material' in it)
        //----------------------------------------------------------------------
        bool hasMaterialBuffer() const;

        //----------------------------------------------------------------------
        // @Return:
        //  List of information about every texture found via reflection.
        //----------------------------------------------------------------------
        const ArrayList<TextureBindingInfo>&    getBoundTextureInfos()  const { return m_boundTextureInfos; }

    protected:
        ID3DBlob*                       m_pShaderBlob           = nullptr;
        ID3D11ShaderReflection*         m_pShaderReflection     = nullptr;
        String                          m_entryPoint            = "main";
        OS::Path                        m_filePath;
        OS::SystemTime                  m_fileTimeAtCompilation;
        ArrayList<ConstantBufferInfo>   m_constantBufferInfos;
        ArrayList<TextureBindingInfo>   m_boundTextureInfos;

        //----------------------------------------------------------------------
        template <typename T>
        bool _CompileFromFile( const OS::Path& path, CString entryPoint )
        {
            m_filePath = path;
            m_entryPoint = entryPoint;
            m_fileTimeAtCompilation = m_filePath.getLastWrittenFileTime();

            return _CompileFromFile( m_filePath, GetLatestProfile<T>().c_str() );
        }

        //----------------------------------------------------------------------
        template <typename T>
        bool _CompileFromSource( const String& source, CString entryPoint )
        {
            m_entryPoint = entryPoint;
            return _CompileFromSource( source, GetLatestProfile<T>().c_str() );
        }

    private:
        //----------------------------------------------------------------------
        bool _CompileFromFile(const OS::Path& path, CString profile);
        bool _CompileFromSource(const String& source, CString profile);

        void _ShaderReflection(ID3DBlob* pShaderBlob);
        void _ReflectResources(const D3D11_SHADER_DESC& shaderDesc);
        void _ReflectConstantBuffer(ID3D11ShaderReflectionConstantBuffer* cb, U32 bindSlot);

        //----------------------------------------------------------------------
        ShaderBase(const ShaderBase& other)               = delete;
        ShaderBase& operator = (const ShaderBase& other)  = delete;
        ShaderBase(ShaderBase&& other)                    = delete;
        ShaderBase& operator = (ShaderBase&& other)       = delete;
    };

    
    // Get the latest profile for the specified Shader type.
    template<class ShaderCompilerClass>
    String GetLatestProfile();

    //----------------------------------------------------------------------
    template<> inline
    String GetLatestProfile<ID3D11VertexShader>()
    {
        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = g_pDevice->GetFeatureLevel();
        switch (featureLevel)
        {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "vs_5_0";
        case D3D_FEATURE_LEVEL_10_1:
            return "vs_4_1";
        case D3D_FEATURE_LEVEL_10_0:
            return "vs_4_0";
        case D3D_FEATURE_LEVEL_9_3:
            return "vs_4_0_level_9_3";
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
            return "vs_4_0_level_9_1";
        }
        return "";
    }

    //----------------------------------------------------------------------
    template<> inline
    String GetLatestProfile<ID3D11PixelShader>()
    {
        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = g_pDevice->GetFeatureLevel();
        switch (featureLevel)
        {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            return "ps_5_0";
        case D3D_FEATURE_LEVEL_10_1:
            return "ps_4_1";
        case D3D_FEATURE_LEVEL_10_0:
            return "ps_4_0";
        case D3D_FEATURE_LEVEL_9_3:
            return "ps_4_0_level_9_3";
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
            return "ps_4_0_level_9_1";
        }
        return "";
    }

} } // End namespaces