#include <vector>
#include <memory>
#include <libyuv.h>
#include <tio/tio_software_frame.h>

using namespace mr::tio;

typedef SoftwareFrameWithMemory (*FuncIntermediateConvert)(const SoftwareFrameWithMemory& frame);

struct FourCCInfo{
    libyuv::FourCC fourcc;
    //NOTE: intermediate format is very important for any to any
    //each format MUST be able to convert to intermediate
    //and intermediate format MUST be able to convert to any others
    //current i420,i422,i444,rgb3/bgr3,argb is the key format
    libyuv::FourCC fourcc_intermediate;
    uint8_t stride_unit_0;
    std::vector<FuncIntermediateConvert> convert_chain_to_intermediate;
};

struct SpecialConvertPair{
    libyuv::FourCC from;
    libyuv::FourCC to;
    std::vector<FuncIntermediateConvert> convert_chain;
};

SoftwareFrameWithMemory yvyu_yuyv_swap(){
    SoftwareFrameWithMemory out;
    return out;
};

//many memory order in convert.h
const FourCCInfo libyuv_fourcc_maps[kSoftwareFrameCount] = {
    [kSoftwareFrameI420   ] = {libyuv::FOURCC_I420, libyuv::FOURCC_I420,  1},
    [kSoftwareFrameYV12   ] = {libyuv::FOURCC_YV12, libyuv::FOURCC_I420,  1},
    [kSoftwareFrameNV12   ] = {libyuv::FOURCC_NV12, libyuv::FOURCC_I420,  1},
    [kSoftwareFrameNV21   ] = {libyuv::FOURCC_NV21, libyuv::FOURCC_I420,  1},

    [kSoftwareFrameI422   ] = {libyuv::FOURCC_I422, libyuv::FOURCC_I422,  1},
    [kSoftwareFrameNV16   ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I422,  1},   //NOT_SUPPORT, MergeUVPlane from I422
    [kSoftwareFrameNV61   ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I422,  1},   //NOT_SUPPORT, MergeUVPlane from I422
    [kSoftwareFrameYUYV422] = {libyuv::FOURCC_YUYV, libyuv::FOURCC_I422,  2},
    [kSoftwareFrameYVYU422] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I422,  2},   //NOT_SUPPORT, YUYV->YVYU USE ABGRToARGB to swap 1|3 channel
    [kSoftwareFrameUYVY422] = {libyuv::FOURCC_UYVY, libyuv::FOURCC_I422,  2},

    [kSoftwareFrameI444   ] = {libyuv::FOURCC_I444, libyuv::FOURCC_I444,  1},
    [kSoftwareFrameNV24   ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I444,  1},   //NOT_SUPPORT, libyuv::MergeUVPlane from I444
    [kSoftwareFrameNV42   ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I444,  1},   //NOT_SUPPORT, libyuv::MergeUVPlane from I444
    [kSoftwareFrameYUV444 ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I444,  3},   //NOT_SUPPORT, libyuv::MergeRGBPlane from I444

    [kSoftwareFrameRGB24  ] = {libyuv::FOURCC_RGB3, libyuv::FOURCC_RGB3,  3},   //RAW (rgb in memory) RAWTO...
    [kSoftwareFrameBGR24  ] = {libyuv::FOURCC_BGR3, libyuv::FOURCC_BGR3,  3},   //24BG (bgr in memory) RGB24To...

    [kSoftwareFrameRGBA32 ] = {libyuv::FOURCC_ABGR, libyuv::FOURCC_ARGB,  4},   //(rgba in memory)
    [kSoftwareFrameBGRA32 ] = {libyuv::FOURCC_ARGB, libyuv::FOURCC_ARGB,  4},   //(bgra in memory)
    [kSoftwareFrameARGB32 ] = {libyuv::FOURCC_BGRA, libyuv::FOURCC_ARGB,  4},   //(argb in memory)
    [kSoftwareFrameABGR32 ] = {libyuv::FOURCC_RGBA, libyuv::FOURCC_ARGB,  4},   //(abgr in memory)

    [kSoftwareFrameGRAY8  ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I420,  1},   //NOT_SUPPORT, need copy to i420 y plane

    [kSoftwareFrameGRAY8A ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_ARGB,  2},   //NOT_SUPPORT, to rgba: splite-uv | u->i420->rgba | a->rgba.a, from rgba: ->i420->y=gray plane | extract a plane | merge y/a plane
};





SoftwareFrameConvert::SoftwareFrameConvert()
{

}

int32_t SoftwareFrameConvert::convert(const SoftwareFrame &source, SoftwareFrame &dest,RotationMode rotate,const CropArea& crop_area)
{
    auto source_fourcc = libyuv_fourcc_maps[source.format];
    auto dest_fourcc = libyuv_fourcc_maps[dest.format];
    auto source_format = source.format;
    auto dest_format = dest.format;
    int32_t crop_width = (crop_area.crop_width == UINT32_MAX)?source.width:crop_area.crop_width;
    int32_t crop_height = (crop_area.crop_height == UINT32_MAX)?source.height:crop_area.crop_height;

    if(dest_format == kSoftwareFrameI420){
        libyuv::ConvertToI420(source.data[0],0,
                dest.data[0],dest.line_size[0],
                dest.data[1],dest.line_size[1],
                dest.data[2],dest.line_size[2],
                crop_area.crop_x,crop_area.crop_y,
                source.line_size[0]/source_fourcc.stride_unit_0, source.height,
                crop_width,crop_height,
                libyuv::kRotate0,
                0);
    }
    else if(dest_format == kSoftwareFrameI422){
       if(source_format == kSoftwareFrameI420){
            libyuv::I420ToI422(source.data[0],source.line_size[0],
                    source.data[1],source.line_size[1],
                    source.data[2],source.line_size[2],
                    dest.data[0],dest.line_size[0],
                    dest.data[1],dest.line_size[1],
                    dest.data[2],dest.line_size[2],
                    source.width,source.height);
        }
        else if(source_format == kSoftwareFrameYV12){
            libyuv::I420ToI422(source.data[0],source.line_size[0],
                    source.data[2],source.line_size[2],
                    source.data[1],source.line_size[1],
                    dest.data[0],dest.line_size[0],
                    dest.data[1],dest.line_size[1],
                    dest.data[2],dest.line_size[2],
                    source.width,source.height);
        }
        else if(source_format == kSoftwareFrameNV12){
           SoftwareFrameWithMemory i420 = {{kSoftwareFrameI420,source.width,source.height}};
           i420.alloc();
           convert(source,i420);
           libyuv::I420ToI422(i420.data[0],i420.line_size[0],
                    i420.data[1],i420.line_size[1],
                    i420.data[2],i420.line_size[2],
                    dest.data[0],dest.line_size[0],
                    dest.data[1],dest.line_size[1],
                    dest.data[2],dest.line_size[2],
                    source.width,source.height);           
        }
         /*else if(source_format == kSoftwareFrameNV21){
            libyuv::SwapUVPlane()
            libyuv::NV12ToI420()
        }
        else if(source_format == kSoftwareFrameI422){
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFrameNV16){
            libyuv::SplitUVPlane()
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFrameNV61){
            libyuv::SplitUVPlane()
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFrameYUYV422){
            libyuv::YUY2ToI422()
        }
        else if(source_format == kSoftwareFrameYVYU422){
            libyuv::ARGBToABGR()
            libyuv::YUY2ToI422()
        }
        else if(source_format == kSoftwareFrameUYVY422){
            libyuv::UYVYToI422()
        }
        else if(source_format == kSoftwareFrameI444){
            libyuv::ScalePlane()
            libyuv::ScalePlane()
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFrameNV24){
            libyuv::SplitUVPlane
            libyuv::ScalePlane()
            libyuv::ScalePlane()
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFrameNV42){
            libyuv::SplitUVPlane
            libyuv::ScalePlane()
            libyuv::ScalePlane()
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFrameYUV444){
            libyuv::SplitRGBPlane()
            libyuv::ScalePlane()
            libyuv::ScalePlane()
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFrameRGB24){
            libyuv::RAWToARGB()
            libyuv::ARGBToI422()
        }
        else if(source_format == kSoftwareFrameBGR24){
            libyuv::RGB24ToARGB()
            libyuv::ARGBToI422()
        }
        else if(source_format == kSoftwareFrameRGBA32){
            libyuv::ABGRToARGB()
            libyuv::ARGBToI422()
        }
        else if(source_format == kSoftwareFrameBGRA32){
            libyuv::ARGBToI422()
        }
        else if(source_format == kSoftwareFrameARGB32){
            libyuv::BGRAToARGB()
            libyuv::ARGBToI422()
        }
        else if(source_format == kSoftwareFrameABGR32){
            libyuv::RGBAToARGB()
            libyuv::ARGBToI422()
        }
        else if(source_format == kSoftwareFrameGRAY8){
            //new i420 frame
            libyuv::CopyPlane()
        }
        else if(source_format == kSoftwareFrameGRAY8A){
            libyuv::SplitUVPlane()
            //new i420 frame
            libyuv::CopyPlane()
            libyuv::I420ToARGB()
            libyuv::ARGBCopyYToAlpha()
        }*/
    }
    return 0;
}
