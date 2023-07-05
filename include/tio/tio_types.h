#ifndef MP_TIO_TYPES_H_
#define MP_TIO_TYPES_H_
#include <cstdint>
#include <cstring>
#include <math.h>
#include <string>
#include <memory>

#define kErrorInvalidFrame       -1
#define kErrorFormatNotMatch     -2
#define kErrorFormatNotSupport   -3
#define kErrorInvalidFrameSize   -4

#define kErrorAllocTexture       -100
#define kErrorInvalidTextureId   -101
#define kErrorInvalidProgram     -102
#define kErrorInvalidGraphicApi  -103


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

#define IS_INTERMEDIATE_FORMAT(format) (format == kSoftwareFormatI420 || format == kSoftwareFormatI422 || format == kSoftwareFormatI444 || format == kSoftwareFormatBGRA32)
#define IS_YUV_INTERLACE_FORMAT(format) (format == kSoftwareFormatYUYV422 || format == kSoftwareFormatYVYU422 || format == kSoftwareFormatUYVY422)

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

enum ShaderType{
    kShaderTypeNone = 0,
    kShaderTypeVertex,
    kShaderTypeFragment,
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

enum SamplerMode : int32_t{
    kSamplerAuto,
    kSamplerNearest,
    kSamplerLinear,
    kSamplerBilinear, //just for software scale
    kSamplerBox, //just for software scale
};

enum FillMode : int32_t{
    kStretchFill,
    kAspectFit,
    kAspectCrop
};


struct FrameArea{
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t width = 0;
    uint32_t height = 0;

    void aspect_crop(float view_width,float view_height,float target_ratio){
        float view_ratio = view_width / view_height;

        if(view_ratio >= target_ratio){
            width = round(view_height * target_ratio);
            height = view_height;
            x = (view_width - width) / 2;
            y = 0;
        }
        else{
            //use view width
            width  = view_width;
            height = round(view_width / target_ratio);
            x = 0;
            y = (view_height - height) / 2;
        }
        return ;
    }
};

struct SoftwareFrame{
    SoftwareFrameFormat format = kSoftwareFormatNone;
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t* data[4] = {0,0,0,0};
    uint32_t linesize[4] = {0,0,0,0};
    void clear(FrameArea area = FrameArea());
};
struct HardwareFrame{
    HardwareFrameFormat format;
    uint32_t width = 0;
    uint32_t height = 0;
    void* context = nullptr;
};

struct GraphicTexture{
    GraphicApi api = kGraphicApiNone;
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
    const char* name = "";
    uint8_t bpp = 0;
    uint8_t planes_count = 0;
    Planer planes[4];
};


static const SoftwareFormatPlaner g_software_format_info[kSoftwareFormatCount] = {
    /*[kSoftwareFormatI420   ] =*/ {"I420",   12,3,{{1,1,1},{0.5,0.5,1},{0.5,0.5,1},{}}},
    /*[kSoftwareFormatYV12   ] =*/ {"YV12",   12,3,{{1,1,1},{0.5,0.5,1},{0.5,0.5,1},{}}},
    /*[kSoftwareFormatNV12   ] =*/ {"NV12",   12,2,{{1,1,1},{0.5,0.5,2},{},{}}},
    /*[kSoftwareFormatNV21   ] =*/ {"NV21",   12,2,{{1,1,1},{0.5,0.5,2},{},{}}},
    /*[kSoftwareFormatI422   ] =*/ {"I422",   16,3,{{1,1,1},{0.5,1,1},{0.5,1,1},{}}},
    /*[kSoftwareFormatNV16   ] =*/ {"NV16",   16,2,{{1,1,1},{0.5,1,2},{},{}}},
    /*[kSoftwareFormatNV61   ] =*/ {"NV61",   16,2,{{1,1,1},{0.5,1,2},{},{}}},
    /*[kSoftwareFormatYUYV422] =*/ {"YUYV422",16,1,{{0.5,1,4},{},{},{}}}, // half sampels of width, but 4ch-32bit per sample,rgba=y1-u-y2-v
    /*[kSoftwareFormatYVYU422] =*/ {"YVYU422",16,1,{{0.5,1,4},{},{},{}}}, // same as above
    /*[kSoftwareFormatUYVY422] =*/ {"UYVY422",16,1,{{0.5,1,4},{},{},{}}}, // same as above
    /*[kSoftwareFormatI444   ] =*/ {"I444",   24,3,{{1,1,1},{1,1,1},{1,1,1},{}}},
    /*[kSoftwareFormatNV24   ] =*/ {"NV24",   24,2,{{1,1,1},{1,1,2},{},{}}},
    /*[kSoftwareFormatNV42   ] =*/ {"NV42",   24,2,{{1,1,1},{1,1,2},{},{}}},
    /*[kSoftwareFormatYUV444 ] =*/ {"YUV444", 24,1,{{1,1,3},{},{},{}}},
    /*[kSoftwareFormatRGB24  ] =*/ {"RGB24",  24,1,{{1,1,3},{},{},{}}},
    /*[kSoftwareFormatBGR24  ] =*/ {"BGR24",  24,1,{{1,1,3},{},{},{}}},
    /*[kSoftwareFormatRGBA32 ] =*/ {"RGBA32", 32,1,{{1,1,4},{},{},{}}},
    /*[kSoftwareFormatBGRA32 ] =*/ {"BGRA32", 32,1,{{1,1,4},{},{},{}}},
    /*[kSoftwareFormatARGB32 ] =*/ {"ARGB32", 32,1,{{1,1,4},{},{},{}}},
    /*[kSoftwareFormatABGR32 ] =*/ {"ABGR32", 32,1,{{1,1,4},{},{},{}}},
    /*[kSoftwareFormatGRAY8  ] =*/ {"GRAY8",  8, 1,{{1,1,1},{},{},{}}},
    /*[kSoftwareFormatGRAY8A ] =*/ {"GRAY8A", 16,1,{{1,1,2},{},{},{}}}
};

static const SoftwareFrameFormat g_software_format_of_channel[5] = {kSoftwareFormatNone,kSoftwareFormatGRAY8,kSoftwareFormatGRAY8A,kSoftwareFormatRGB24,kSoftwareFormatRGBA32};

struct SoftwareFrameWithMemory : public SoftwareFrame{

public:
    SoftwareFrameWithMemory(){}
    SoftwareFrameWithMemory(const SoftwareFrameWithMemory& other);
    SoftwareFrameWithMemory(SoftwareFrameFormat format,uint32_t width, uint32_t height);
    SoftwareFrameWithMemory(SoftwareFrameFormat format,uint32_t width, uint32_t height,uint8_t* data);
    //alloc buffer and fill data,linesize
    SoftwareFrameWithMemory& alloc();
    //attach buffer must be no padding linesize
    SoftwareFrameWithMemory& attach(uint8_t* data);
    SoftwareFrameWithMemory& clone_from(const SoftwareFrameWithMemory& data);
    SoftwareFrameWithMemory clone_new();
private:
    void fill_plane(uint8_t* data_from = nullptr);
public:
    uint32_t plane_size[4];
    uint8_t* data_buffer_ = nullptr;
    std::shared_ptr<uint8_t> frame_memory_;
};

struct ReferenceShader{
    struct RenderParam{
        int32_t view_x      = 0;
        int32_t view_y      = 0;
        int32_t view_width  = 0;
        int32_t view_height = 0;
        float   rotate      = 0;
        float   scale_x     = 1;
        float   scale_y     = 1;
        float   offset_x    = 0;
        float   offset_y    = 0;
        bool    ingore_source_aspect_ = false;
        void fill_with(FillMode mode,int32_t width,int32_t height,int32_t content_width,int32_t content_height){
            view_x      = 0;
            view_y      = 0;
            view_width  = 0;
            view_height = 0;
            rotate      = 0;
            scale_x     = 1;
            scale_y     = 1;
            offset_x    = 0;
            offset_y    = 0;
            ingore_source_aspect_ = true;
            if(mode == kStretchFill){
                view_width = width;
                view_height = height;
            }
            else if(mode == kAspectFit){
                FrameArea area;
                area.aspect_crop(width,height,float(content_width)/content_height);
                view_x      = area.x;
                view_y      = area.y;
                view_width  = area.width;
                view_height = area.height;
            }
            else if(mode == kAspectCrop){
                FrameArea area;
                area.aspect_crop(content_width,content_height,float(width)/height);

                view_width  = width;
                view_height = height;
                scale_x     = float(content_width) / area.width;
                scale_y     = float(content_height) / area.height;
                offset_x    = float(area.x) / content_width;
                offset_y    = float(area.y) / content_height;
            }
        }
    };
    virtual ~ReferenceShader();
    virtual std::string shader(ShaderType type) = 0;
    virtual uint64_t program() = 0;
    virtual SoftwareFrameFormat format() = 0;
    virtual GraphicApi graphic_api() = 0;
    virtual int32_t use() = 0;
    virtual int32_t render(const GraphicTexture& textures,const RenderParam& param) = 0;
    virtual int32_t render(const SoftwareFrame &frame, const RenderParam &param,SamplerMode sampler_mode = kSamplerAuto) = 0;
    virtual int32_t render(const RenderParam &param) = 0;
};

template<typename T>
inline T _tio_max_align(T size,uint8_t div_max = 8){
    while(size % div_max) div_max >>= 1;
    return div_max;
}
}//namespace tio
}//namespace mountain-ripper


#endif // TIO_TYPES_H
