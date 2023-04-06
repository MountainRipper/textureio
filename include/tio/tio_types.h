#ifndef MP_TIO_TYPES_H_
#define MP_TIO_TYPES_H_
#include <cstdint>
#include <string>
#include <memory>

#define kErrorInvalidTextureId -1

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
    kColorSpaceBt709,
    kColorSpaceBt2020,
    kColorSpaceBt2100
};

enum RotationMode : int32_t{
  kRotate0 = 0,
  kRotate90 = 90,
  kRotate180 = 180,
  kRotate270 = 270
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
    uint64_t context[4] = {0,0,0,0};
    //OpenGL: Texture unit, GL_TEXTURE[0...n]
    uint64_t flags[4] = {0,0,0,0};
};

struct Planer{
    float scale_x = 0;
    float scale_y = 0;
    uint8_t channels = 0;
};
struct SoftwareFormatPlaner{
    uint8_t count = 0;
    Planer planes[4];
};

struct CropArea{
    uint32_t crop_x = 0;
    uint32_t crop_y = 0;
    uint32_t crop_width = UINT32_MAX;
    uint32_t crop_height = UINT32_MAX;
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
static const uint8_t g_soft_format_bpps[kSoftwareFormatCount]={12,12,12,12,/*420*/
                                                          16,16,16,16,16,16,/*422*/
                                                          24,24,24,24,/*444*/
                                                          24,24,/*rgb*/
                                                          32,32,32,32,/*rgb*/
                                                          8,/*gray*/
                                                          16,/*gray alpaha*/
};

static const SoftwareFormatPlaner g_software_format_planers[kSoftwareFormatCount] = {
    [kSoftwareFormatI420   ] = {3,{{1,1,1},{0.5,0.5,1},{0.5,0.5,1},{}}},
    [kSoftwareFormatYV12   ] = {3,{{1,1,1},{0.5,0.5,1},{0.5,0.5,1},{}}},
    [kSoftwareFormatNV12   ] = {2,{{1,1,1},{0.5,0.5,2},{},{}}},
    [kSoftwareFormatNV21   ] = {2,{{1,1,1},{0.5,0.5,2},{},{}}},

    [kSoftwareFormatI422   ] = {3,{{1,1,1},{0.5,1,1},{0.5,1,1},{}}},
    [kSoftwareFormatNV16   ] = {2,{{1,1,1},{0.5,1,2},{},{}}},
    [kSoftwareFormatNV61   ] = {2,{{1,1,1},{0.5,1,2},{},{}}},
    [kSoftwareFormatYUYV422] = {1,{{0.5,1,4},{},{},{}}}, // 0.5 sampels of width, but 4ch-32bit per sample,rgba=y1-u-y2-v
    [kSoftwareFormatYVYU422] = {1,{{0.5,1,4},{},{},{}}}, // same as above
    [kSoftwareFormatUYVY422] = {1,{{0.5,1,4},{},{},{}}}, // same as above

    [kSoftwareFormatI444   ] = {3,{{1,1,1},{1,1,1},{1,1,1},{}}},
    [kSoftwareFormatNV24   ] = {2,{{1,1,1},{1,1,2},{},{}}},
    [kSoftwareFormatNV42   ] = {2,{{1,1,1},{1,1,2},{},{}}},
    [kSoftwareFormatYUV444 ] = {1,{{1,1,3},{},{},{}}},

    [kSoftwareFormatRGB24  ] = {1,{{1,1,3},{},{},{}}},
    [kSoftwareFormatBGR24  ] = {1,{{1,1,3},{},{},{}}},

    [kSoftwareFormatRGBA32 ] = {1,{{1,1,4},{},{},{}}},
    [kSoftwareFormatBGRA32 ] = {1,{{1,1,4},{},{},{}}},
    [kSoftwareFormatARGB32 ] = {1,{{1,1,4},{},{},{}}},
    [kSoftwareFormatABGR32 ] = {1,{{1,1,4},{},{},{}}},

    [kSoftwareFormatGRAY8  ] = {1,{{1,1,1},{},{},{}}},

    [kSoftwareFormatGRAY8A ] = {1,{{1,1,2},{},{},{}}}
};


struct SoftwareFrameWithMemory : public SoftwareFrame{

public:
    //alloc buffer and fill data,linesize
    void alloc(){        
        uint32_t bpp = g_soft_format_bpps[format];
        uint32_t bytes = width*height*bpp/8;
        frame_memory_ = std::shared_ptr<uint8_t>(new uint8_t[bytes],std::default_delete<uint8_t[]>());
        data_buffer_ = frame_memory_.get();
        fill_plane();
    }
    void attach(uint8_t* data){
        data_buffer_ = data;
        fill_plane();
    }
public:
    uint32_t plane_size[4];
    uint8_t* data_buffer_ = nullptr;
    std::shared_ptr<uint8_t> frame_memory_;
private:
    void fill_plane(){
        uint8_t depth = 8;
        line_size[0] = line_size[1] = line_size[2] = line_size[3] = 0;
        data[0] = data[1] = data[2] = data[3] = nullptr;
        /*if(format == kSoftwareFormatI420 ||
           format == kSoftwareFormatYV12){
            line_size[0] = width;
            line_size[1] = line_size[2] = width/2;

            data[0] = data_buffer_;
            auto plane_u = data[0] + width * height;
            auto plane_v = data[1] + width * height / 4;
            std::swap(plane_u,plane_v);
            data[1] = plane_u;
            data[2] = plane_v;
        }
        else if(format == kSoftwareFormatNV12 ||
                format == kSoftwareFormatNV21){
            line_size[0] = width;
            line_size[1] = width;

            data[0] = data_buffer_;
            data[1] = data[0] + width * height;
        }
        else if(format == kSoftwareFormatI422){
            line_size[0] = width;
            line_size[1] = width/2;
            line_size[2] = width/2;

            data[0] = data_buffer_;
            data[1] = data[0] + width * height;
            data[2] = data[1] + width * height / 2;
        }
        else if(format == kSoftwareFormatNV16 ||
                format == kSoftwareFormatNV61){
            line_size[0] = width;
            line_size[1] = width;

            data[0] = data_buffer_;
            data[1] = data[0] + width * height;
        }
        else if(format == kSoftwareFormatYUYV422 ||
                format == kSoftwareFormatYVYU422 ||
                format == kSoftwareFormatUYVY422){
            line_size[0] = width*2;
            data[0] = data_buffer_;
        }
        else if(format == kSoftwareFormatI444){
            line_size[0] = line_size[1] = line_size[2] = width;
        }
        else if(format == kSoftwareFormatNV24 ||
                format == kSoftwareFormatNV42){
            line_size[0] = width;
            line_size[1] = width*2;

            data[0] = data_buffer_;
            data[1] = data[0] + width * height;
        }
        else if(format == kSoftwareFormatYUV444 ||
                format == kSoftwareFormatRGB24  ||
                format == kSoftwareFormatBGR24){
            line_size[0] = width*3;
            data[0] = data_buffer_;
        }
        else if(format == kSoftwareFormatRGBA32 ||
                format == kSoftwareFormatBGRA32 ||
                format == kSoftwareFormatARGB32 ||
                format == kSoftwareFormatABGR32){
            line_size[0] = width*4;
            data[0] = data_buffer_;
        }
        else if(format == kSoftwareFormatGRAY8){
            line_size[0] = width*1;
            data[0] = data_buffer_;
        }
        else if(format == kSoftwareFormatGRAY8A){
            line_size[0] = width*2;
            data[0] = data_buffer_;
        }*/
        uint8_t* plane_ptr = data_buffer_;
        auto& planes = g_software_format_planers[format].planes;
        for(int index = 0; index < 4; index++){
            auto& plane = planes[index];
            if(plane.channels == 0)
                break;
            line_size[index] = (width * plane.scale_x) * plane.channels * depth / 8;
            plane_size[index] = (height * plane.scale_y) * line_size[index];
            data[index] = plane_ptr;
            plane_ptr += plane_size[index];
        }
    }
};

template<typename T>
inline T _tio_max_align(T size,uint8_t div_max = 8){
    while(size % div_max) div_max >>= 1;
    return div_max;
}
}//namespace tio
}//namespace mountain-ripper


#endif // TIO_TYPES_H
