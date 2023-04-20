#ifndef MP_TIO_TYPES_H_
#define MP_TIO_TYPES_H_
#include <cstdint>
#include <cstring>
#include <math.h>
#include <string>
#include <memory>

#define kErrorInvalidFrame     -1
#define kErrorInvalidTextureId -2
#define kErrorFormatNotMatch   -3
#define kErrorAllocTexture     -4

namespace mr {
namespace tio {


enum SoftwareFrameFormat: int32_t{
    kSoftwareFormatNone = -1,
    kSoftwareFormatI420,     ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    kSoftwareFormatYV12,     ///< as above, but U and V Planer are swapped
    kSoftwareFormatNV12,     ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    kSoftwareFormatNV21,     ///< as above, but U and V bytes are swapped

    kSoftwareFormatI422,     ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    kSoftwareFormatNV16,     ///< planar YUV 4:2:2, 16bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    kSoftwareFormatNV61,     ///< as above, but U and V bytes are swapped
    kSoftwareFormatYUYV422,  ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
    kSoftwareFormatYVYU422,  ///< packed YUV 4:2:2, 16bpp, Y0 Cr Y1 Cb
    kSoftwareFormatUYVY422,  ///< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1

    kSoftwareFormatI444,     ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
    kSoftwareFormatNV24,     ///< planar YUV 4:4:4, 24bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    kSoftwareFormatNV42,     ///< as above, but U and V bytes are swapped
    kSoftwareFormatYUV444,   ///< packed YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)

    kSoftwareFormatRGB24,    ///< packed RGB 8:8:8, 24bpp, RGBRGB...
    kSoftwareFormatBGR24,    ///< packed RGB 8:8:8, 24bpp, BGRBGR...

    kSoftwareFormatRGBA32,   ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
    kSoftwareFormatBGRA32,   ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...
    kSoftwareFormatARGB32,   ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
    kSoftwareFormatABGR32,   ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...

    kSoftwareFormatGRAY8,    ///<        Y        ,  8bpp , used for 1 channel color, for example GL_RED
    kSoftwareFormatGRAY8A,   ///< 8 bits gray, 8 bits alpha, used for  2 channel color ,for example GL_RG

    //values
    kSoftwareFormatCount,
    kSoftwareFormatFirst = kSoftwareFormatI420,
    kSoftwareFormatYUVStart = kSoftwareFormatI420,
    kSoftwareFormatYUVEnd = kSoftwareFormatYUV444,
};

enum HardwareFrameFormat: int32_t{
    kHardwareFrameNone = 0,
    //linux
    kHardwareFrameVaapiSurface,///< VA-API
    kHardwareFrameVdpauSurface,///< VDPAU
    kHardwareFrameDrmPrime,///< drm/kms prime buffer, some soc such as rockchip's rkmpp use this way
    //windows
    kHardwareFrameD3D11,///< ID3D11Texture2D
    kHardwareFrameD3D11VLD,///< ID3D11VideoDecoderOutputView
    kHardwareFrameDxva2,///< LPDIRECT3DSURFACE9

    //apple
    kHardwareFrameVideoToolBox, ///< hardware decoding through apple Videotoolbox

    //android
    kHardwareFrameMediaCodec,///< hardware decoding through MediaCodec,?? android decode to memory
    kHardwareFrameHardwareBuffer,///< HardwareBuffer from API-26

    //cuda
    kHardwareFrameCuda,///NVDEC
};

enum GraphicApi : int32_t{
    kGraphicApiNone = 0,
    kGraphicApiOpenGL, //auto check running in gles env.
    kGraphicApiDirect3D,
    kGraphicApiMetal,
    kGraphicApiVulkan,
};

enum YuvColorSpace : int32_t{
    kColorSpaceNone = 0,
    kColorSpaceBt601,
    kColorSpaceBt601Full, //AKA JPEG
    kColorSpaceBt709,
    kColorSpaceBt709Full,
    kColorSpaceBt2020,
    kColorSpaceBt2020Full
};

enum RotationMode : int32_t{
  kRotate0 = 0,
  kRotate90 = 90,
  kRotate180 = 180,
  kRotate270 = 270
};

enum SamplerMode: int32_t{
    kSamplerAuto,
    kSamplerNearest,
    kSamplerLinear,
    kSamplerBilinear, //just for software scale
    kSamplerBox, //just for software scale
};

struct SoftwareFrame{
    SoftwareFrameFormat format = kSoftwareFormatNone;
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t* data[4] = {0,0,0,0};
    uint32_t line_size[4] = {0,0,0,0};
};
struct HardwareFrame{
    HardwareFrameFormat format;
    uint32_t width = 0;
    uint32_t height = 0;
    void* context{nullptr};
};

struct GraphicTexture{
    GraphicApi api = kGraphicApiNone;
    //if provide,auto bind uploaded texture to program
    uint64_t program = 0;
    //OpenGL: texture id
    uint64_t context[4] = {0,0,0,0};
    //OpenGL: texture unit x, use as 'GL_TEXTURE0 + x'
    uint64_t flags[4] = {0,0,0,0};

    uint32_t width  = 0;
    uint32_t height = 0;
};

struct Planer{
    float scale_x = 0;
    float scale_y = 0;
    uint8_t channels = 0;
};
struct SoftwareFormatPlaner{
    uint8_t count = 0;
    uint8_t bpp = 0;
    Planer planes[4];
};

struct FrameArea{
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t width = UINT32_MAX;
    uint32_t height = UINT32_MAX;

    void aspect_fit(float view_width,float view_height,float target_ratio){

        float view_ratio = view_width / view_height;

        if(view_ratio >= target_ratio){
            width  = round(view_height * target_ratio);
            height = view_height;
            x = (view_width - width) / 2;
            y = 0;
        }
        else{
            width = view_width;
            height = round(view_width / target_ratio);
            x = 0;
            y = (view_height - height) / 2;
        }
    }

    void aspect_crop(float view_width,float view_height,float target_ratio){
        float view_ratio = view_width / view_height;

        if(view_ratio >= target_ratio){
            width = round(view_height * target_ratio);
            height = view_height;
            x = (view_width - width) / 2; //it will outside of view
            y = 0;
        }
        else{
            //use view width
            width  = view_width;
            height = round(view_width / target_ratio);
            x = 0;
            y = (view_height - height) / 2;  //it will outside of view
        }
        return ;
    }
};
static const char* g_soft_format_names[kSoftwareFormatCount] = {
   "I420",
   "YV12",
   "NV12",
   "NV21",

   "I422",
   "NV16",
   "NV61",
   "YUYV422",
   "YVYU422",
   "UYVY422",

   "I444",
   "NV24",
   "NV42",
   "YUV444",

   "RGB24",
   "BGR24",

   "RGBA32",
   "BGRA32",
   "ARGB32",
   "ABGR32",

   "GRAY8",
   "GRAY8A",
};


static const SoftwareFormatPlaner g_software_format_planers[kSoftwareFormatCount] = {
    [kSoftwareFormatI420   ] = {3,12,{{1,1,1},{0.5,0.5,1},{0.5,0.5,1},{}}},
    [kSoftwareFormatYV12   ] = {3,12,{{1,1,1},{0.5,0.5,1},{0.5,0.5,1},{}}},
    [kSoftwareFormatNV12   ] = {2,12,{{1,1,1},{0.5,0.5,2},{},{}}},
    [kSoftwareFormatNV21   ] = {2,12,{{1,1,1},{0.5,0.5,2},{},{}}},

    [kSoftwareFormatI422   ] = {3,16,{{1,1,1},{0.5,1,1},{0.5,1,1},{}}},
    [kSoftwareFormatNV16   ] = {2,16,{{1,1,1},{0.5,1,2},{},{}}},
    [kSoftwareFormatNV61   ] = {2,16,{{1,1,1},{0.5,1,2},{},{}}},
    [kSoftwareFormatYUYV422] = {1,16,{{0.5,1,4},{},{},{}}}, // 0.5 sampels of width, but 4ch-32bit per sample,rgba=y1-u-y2-v
    [kSoftwareFormatYVYU422] = {1,16,{{0.5,1,4},{},{},{}}}, // same as above
    [kSoftwareFormatUYVY422] = {1,16,{{0.5,1,4},{},{},{}}}, // same as above

    [kSoftwareFormatI444   ] = {3,24,{{1,1,1},{1,1,1},{1,1,1},{}}},
    [kSoftwareFormatNV24   ] = {2,24,{{1,1,1},{1,1,2},{},{}}},
    [kSoftwareFormatNV42   ] = {2,24,{{1,1,1},{1,1,2},{},{}}},
    [kSoftwareFormatYUV444 ] = {1,24,{{1,1,3},{},{},{}}},

    [kSoftwareFormatRGB24  ] = {1,24,{{1,1,3},{},{},{}}},
    [kSoftwareFormatBGR24  ] = {1,24,{{1,1,3},{},{},{}}},

    [kSoftwareFormatRGBA32 ] = {1,32,{{1,1,4},{},{},{}}},
    [kSoftwareFormatBGRA32 ] = {1,32,{{1,1,4},{},{},{}}},
    [kSoftwareFormatARGB32 ] = {1,32,{{1,1,4},{},{},{}}},
    [kSoftwareFormatABGR32 ] = {1,32,{{1,1,4},{},{},{}}},

    [kSoftwareFormatGRAY8  ] = {1,8,{{1,1,1},{},{},{}}},

    [kSoftwareFormatGRAY8A ] = {1,16,{{1,1,2},{},{},{}}}
};


struct SoftwareFrameWithMemory : public SoftwareFrame{

public:
    SoftwareFrameWithMemory(){}
    SoftwareFrameWithMemory(const SoftwareFrameWithMemory& other);
    SoftwareFrameWithMemory(SoftwareFrameFormat format,uint32_t width, uint32_t height);
    SoftwareFrameWithMemory(SoftwareFrameFormat format,uint32_t width, uint32_t height,uint8_t* data);
    //alloc buffer and fill data,linesize
    void alloc();
    //attach buffer must be no padding linesize
    void attach(uint8_t* data);
    void clone_from(const SoftwareFrameWithMemory& data);
    SoftwareFrameWithMemory clone_new();
private:
    void fill_plane(uint8_t* data_from = nullptr);
public:
    uint32_t plane_size[4];
    uint8_t* data_buffer_ = nullptr;
    std::shared_ptr<uint8_t> frame_memory_;
};


template<typename T>
inline T _tio_max_align(T size,uint8_t div_max = 8){
    while(size % div_max) div_max >>= 1;
    return div_max;
}
}//namespace tio
}//namespace mountain-ripper


#endif // TIO_TYPES_H
