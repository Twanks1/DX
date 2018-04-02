#include "utils.h"
/**********************************************************************
    class: None (utils.h)

    author: S. Hau
    date: March 31, 2018
**********************************************************************/

namespace Graphics {

    //----------------------------------------------------------------------
    U32 ByteCountFromTextureFormat(TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::Alpha8:         return 1; break;
        case TextureFormat::ARGB4444:       return 2; break;
        case TextureFormat::RGBA32:         return 4; break;
        case TextureFormat::RGB565:         return 2; break;
        case TextureFormat::R16:            return 2; break;
        case TextureFormat::BGRA32:         return 4; break;
        case TextureFormat::RHalf:          return 2; break;
        case TextureFormat::RGHalf:         return 4; break;
        case TextureFormat::RGBAHalf:       return 8; break;
        case TextureFormat::RFloat:         return 4; break;
        case TextureFormat::RGFloat:        return 8; break;
        case TextureFormat::RGBAFloat:      return 16; break;
        case TextureFormat::YUY2:           return 4; break;
        case TextureFormat::RGB9e5Float:    return 4; break;
        case TextureFormat::RG16:           return 2; break;
        case TextureFormat::R8:             return 1; break;
        case TextureFormat::BC4:
        case TextureFormat::BC5:
        case TextureFormat::BC6H:
        case TextureFormat::BC7:
        default:
        ASSERT( false );
        };
        return 0;
    }

}

