#ifndef VIDEO2HW_H
#define VIDEO2HW_H
#include <cstdint>
enum VideoFormat: int32_t{
    kVideoFormatYUV420P,///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    kVideoFormatNV12,///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    kVideoFormatNV21,///< as above, but U and V bytes are swapped
    kVideoFormatYUYV422,///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
    kVideoFormatUYVY422,///< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
    kVideoFormatYUV444,//< packed YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
    kVideoFormatYUV444P,///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
    kVideoFormatNV24,///< planar YUV 4:4:4, 24bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    kVideoFormatNV42,///< as above, but U and V bytes are swapped

    kVideoFormatRGB24,///< packed RGB 8:8:8, 24bpp, RGBRGB...
    kVideoFormatBGR24,///< packed RGB 8:8:8, 24bpp, BGRBGR...
    kVideoFormatRGBA32,///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
    kVideoFormatBGRA32,///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...
    kVideoFormatARGB32,///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
    kVideoFormatABGR32,///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
    kVideoFormatGRAY8,///<        Y        ,  8bpp , used for 1 channel color, for example GL_RED
    kVideoFormatGRAY8A,///< 8 bits gray, 8 bits alpha, used for  2 channel color ,for example GL_RG

    //linux
    kVideoFormatVaapiSurface,///VA-API
    kVideoFormatVdpauSurface,///VDPAU
    kVideoFormatDrmPrime,///drm/kms prime buffer, some soc such as rockchip's rkmpp use this way
    //windows
    kVideoFormatD3D11,///ID3D11Texture2D
    kVideoFormatD3D11VLD,///ID3D11VideoDecoderOutputView
    kVideoFormatDxva2,///LPDIRECT3DSURFACE9

    //apple
    kVideoFormatVideoToolBox, ///< hardware decoding through apple Videotoolbox

    //android
    kVideoFormatMediaCodec,///< hardware decoding through MediaCodec

    //cuda
    kVideoFormatCuda,///NVDEC
};

enum GraphicApi : int32_t{
    kGraphicApiOpenGL,
    kGraphicApiOpenDirect3D,
    kGraphicApiMetal,
    kGraphicApiVulkan,
};

class TextureIO
{
public:
    TextureIO();


};

#endif // VIDEO2HW_H
