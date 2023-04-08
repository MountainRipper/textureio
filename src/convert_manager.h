#ifndef CONVERT_DEFINES_H
#define CONVERT_DEFINES_H

#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <libyuv.h>
#include <tio_types.h>

using namespace mr::tio;

struct SoftwareFormatMapping{
    libyuv::FourCC fourcc;
    //NOTE: intermediate format is very important for any to any
    //each format MUST be able to convert to intermediate
    //and intermediate format MUST be able to convert to any others
    //current i420,i422,i444,bgra is the key format
    SoftwareFrameFormat intermediate_format;
    uint8_t stride_unit_0;
};


//many memory order in convert.h
const static SoftwareFormatMapping g_software_format_maps[kSoftwareFormatCount] = {
    [kSoftwareFormatI420   ] = {libyuv::FOURCC_I420, kSoftwareFormatI420  ,  1},
    [kSoftwareFormatYV12   ] = {libyuv::FOURCC_YV12, kSoftwareFormatI420  ,  1},
    [kSoftwareFormatNV12   ] = {libyuv::FOURCC_NV12, kSoftwareFormatI420  ,  1},
    [kSoftwareFormatNV21   ] = {libyuv::FOURCC_NV21, kSoftwareFormatI420  ,  1},

    [kSoftwareFormatI422   ] = {libyuv::FOURCC_I422, kSoftwareFormatI422  ,  1},
    [kSoftwareFormatNV16   ] = {libyuv::FOURCC_ANY , kSoftwareFormatI422  ,  1},   //NOT_SUPPORT, MergeUVPlane from I422
    [kSoftwareFormatNV61   ] = {libyuv::FOURCC_ANY , kSoftwareFormatI422  ,  1},   //NOT_SUPPORT, MergeUVPlane from I422
    [kSoftwareFormatYUYV422] = {libyuv::FOURCC_YUYV, kSoftwareFormatI422  ,  2},
    [kSoftwareFormatYVYU422] = {libyuv::FOURCC_ANY , kSoftwareFormatI422  ,  2},   //NOT_SUPPORT, YUYV->YVYU USE ABGRToARGB to swap 1|3 channel
    [kSoftwareFormatUYVY422] = {libyuv::FOURCC_UYVY, kSoftwareFormatI422  ,  2},

    [kSoftwareFormatI444   ] = {libyuv::FOURCC_I444, kSoftwareFormatI444  ,  1},
    [kSoftwareFormatNV24   ] = {libyuv::FOURCC_ANY , kSoftwareFormatI444  ,  1},   //NOT_SUPPORT, libyuv::MergeUVPlane from I444
    [kSoftwareFormatNV42   ] = {libyuv::FOURCC_ANY , kSoftwareFormatI444  ,  1},   //NOT_SUPPORT, libyuv::MergeUVPlane from I444
    [kSoftwareFormatYUV444 ] = {libyuv::FOURCC_ANY , kSoftwareFormatI444  ,  3},   //NOT_SUPPORT, libyuv::MergeRGBPlane from I444

    [kSoftwareFormatRGB24  ] = {libyuv::FOURCC_RGB3, kSoftwareFormatBGRA32,  3},   //RAW (rgb in memory) RAWTo...
    [kSoftwareFormatBGR24  ] = {libyuv::FOURCC_BGR3, kSoftwareFormatBGRA32,  3},   //24BG (bgr in memory) RGB24To...

    [kSoftwareFormatRGBA32 ] = {libyuv::FOURCC_ABGR, kSoftwareFormatBGRA32,  4},   //(rgba in memory)
    [kSoftwareFormatBGRA32 ] = {libyuv::FOURCC_ARGB, kSoftwareFormatBGRA32,  4},   //(bgra in memory)
    [kSoftwareFormatARGB32 ] = {libyuv::FOURCC_BGRA, kSoftwareFormatBGRA32,  4},   //(argb in memory)
    [kSoftwareFormatABGR32 ] = {libyuv::FOURCC_RGBA, kSoftwareFormatBGRA32,  4},   //(abgr in memory)

    [kSoftwareFormatGRAY8  ] = {libyuv::FOURCC_ANY , kSoftwareFormatI420  ,  1},   //NOT_SUPPORT, need copy to i420 y plane

    [kSoftwareFormatGRAY8A ] = {libyuv::FOURCC_ANY , kSoftwareFormatBGRA32,  2},   //NOT_SUPPORT, to rgba: splite-uv | u->i420->rgba | a->rgba.a, from rgba: ->i420->y=gray plane | extract a plane | merge y/a plane
};

typedef std::function<int32_t(
        const SoftwareFrame&,
        SoftwareFrame&,
        RotationMode,
        const CropArea&)> Converter;

class ConvertManager{
public:
    static void init();
    static void add_converter(SoftwareFrameFormat input,SoftwareFrameFormat output,Converter converter);
    static Converter get_convertor(SoftwareFrameFormat input,SoftwareFrameFormat output);
    static std::map<uint32_t,Converter> converters_;
};


#endif // CONVERT_DEFINES_H
