#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <libyuv.h>
#include <tio/tio_software_frame.h>

using namespace mr::tio;
struct FourCCInfo{
    libyuv::FourCC fourcc;
    //NOTE: intermediate format is very important for any to any
    //each format MUST be able to convert to intermediate
    //and intermediate format MUST be able to convert to any others
    //current i420,i422,i444,argb is the key format
    libyuv::FourCC fourcc_intermediate;
    uint8_t stride_unit_0;
};


//many memory order in convert.h
const FourCCInfo libyuv_fourcc_maps[kSoftwareFormatCount] = {
    [kSoftwareFormatI420   ] = {libyuv::FOURCC_I420, libyuv::FOURCC_I420,  1},
    [kSoftwareFormatYV12   ] = {libyuv::FOURCC_YV12, libyuv::FOURCC_I420,  1},
    [kSoftwareFormatNV12   ] = {libyuv::FOURCC_NV12, libyuv::FOURCC_I420,  1},
    [kSoftwareFormatNV21   ] = {libyuv::FOURCC_NV21, libyuv::FOURCC_I420,  1},

    [kSoftwareFormatI422   ] = {libyuv::FOURCC_I422, libyuv::FOURCC_I422,  1},
    [kSoftwareFormatNV16   ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I422,  1},   //NOT_SUPPORT, MergeUVPlane from I422
    [kSoftwareFormatNV61   ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I422,  1},   //NOT_SUPPORT, MergeUVPlane from I422
    [kSoftwareFormatYUYV422] = {libyuv::FOURCC_YUYV, libyuv::FOURCC_I422,  2},
    [kSoftwareFormatYVYU422] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I422,  2},   //NOT_SUPPORT, YUYV->YVYU USE ABGRToARGB to swap 1|3 channel
    [kSoftwareFormatUYVY422] = {libyuv::FOURCC_UYVY, libyuv::FOURCC_I422,  2},

    [kSoftwareFormatI444   ] = {libyuv::FOURCC_I444, libyuv::FOURCC_I444,  1},
    [kSoftwareFormatNV24   ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I444,  1},   //NOT_SUPPORT, libyuv::MergeUVPlane from I444
    [kSoftwareFormatNV42   ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I444,  1},   //NOT_SUPPORT, libyuv::MergeUVPlane from I444
    [kSoftwareFormatYUV444 ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I444,  3},   //NOT_SUPPORT, libyuv::MergeRGBPlane from I444

    [kSoftwareFormatRGB24  ] = {libyuv::FOURCC_RGB3, libyuv::FOURCC_RGB3,  3},   //RAW (rgb in memory) RAWTO...
    [kSoftwareFormatBGR24  ] = {libyuv::FOURCC_BGR3, libyuv::FOURCC_BGR3,  3},   //24BG (bgr in memory) RGB24To...

    [kSoftwareFormatRGBA32 ] = {libyuv::FOURCC_ABGR, libyuv::FOURCC_ARGB,  4},   //(rgba in memory)
    [kSoftwareFormatBGRA32 ] = {libyuv::FOURCC_ARGB, libyuv::FOURCC_ARGB,  4},   //(bgra in memory)
    [kSoftwareFormatARGB32 ] = {libyuv::FOURCC_BGRA, libyuv::FOURCC_ARGB,  4},   //(argb in memory)
    [kSoftwareFormatABGR32 ] = {libyuv::FOURCC_RGBA, libyuv::FOURCC_ARGB,  4},   //(abgr in memory)

    [kSoftwareFormatGRAY8  ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_I420,  1},   //NOT_SUPPORT, need copy to i420 y plane

    [kSoftwareFormatGRAY8A ] = {libyuv::FOURCC_ANY , libyuv::FOURCC_ARGB,  2},   //NOT_SUPPORT, to rgba: splite-uv | u->i420->rgba | a->rgba.a, from rgba: ->i420->y=gray plane | extract a plane | merge y/a plane
};

typedef std::function<int32_t(
        SoftwareFrameWithMemory&,
        SoftwareFrameWithMemory&,
        RotationMode,
        const CropArea&)> Converter;

class FixedConverters{
public:
    static void init(){
        static bool initialized = false;
        if(initialized)
            return;
        initialized = true;

        Converter convert_to_i420 = [](
                SoftwareFrameWithMemory& source,
                SoftwareFrameWithMemory& dest,
                RotationMode rotate,
                const CropArea& crop_area )->int32_t{
            auto source_fourcc = libyuv_fourcc_maps[source.format];
            auto dest_fourcc = libyuv_fourcc_maps[dest.format];
            int32_t crop_width = (crop_area.crop_width == UINT32_MAX)?source.width:crop_area.crop_width;
            int32_t crop_height = (crop_area.crop_height == UINT32_MAX)?source.height:crop_area.crop_height;

            libyuv::ConvertToI420(source.data[0],0,
                    dest.data[0],dest.line_size[0],
                    dest.data[1],dest.line_size[1],
                    dest.data[2],dest.line_size[2],
                    crop_area.crop_x,crop_area.crop_y,
                    source.line_size[0]/source_fourcc.stride_unit_0, source.height,
                    crop_width,crop_height,
                    libyuv::kRotate0,
                    0);
            return  0;
        };

        add_converter(kSoftwareFormatI420,kSoftwareFormatI422,
                      [](SoftwareFrameWithMemory& source,
                         SoftwareFrameWithMemory& dest,
                         RotationMode rotate,
                         const CropArea& crop_area)->int32_t{
            libyuv::I420ToI422(source.data[0],source.line_size[0],
                    source.data[1],source.line_size[1],
                    source.data[2],source.line_size[2],
                    dest.data[0],dest.line_size[0],
                    dest.data[1],dest.line_size[1],
                    dest.data[2],dest.line_size[2],
                    source.width,source.height);
            return  0;
        });
        add_converter(kSoftwareFormatYV12,kSoftwareFormatI422,
                      [](SoftwareFrameWithMemory& source,
                         SoftwareFrameWithMemory& dest,
                         RotationMode rotate,
                         const CropArea& crop_area)->int32_t{
            libyuv::I420ToI422(source.data[0],source.line_size[0],
                    source.data[2],source.line_size[2],
                    source.data[1],source.line_size[1],
                    dest.data[0],dest.line_size[0],
                    dest.data[1],dest.line_size[1],
                    dest.data[2],dest.line_size[2],
                    source.width,source.height);
            return  0;
        });
        add_converter(kSoftwareFormatNV12,kSoftwareFormatI422,
                      [](SoftwareFrameWithMemory& source,
                         SoftwareFrameWithMemory& dest,
                         RotationMode rotate,
                         const CropArea& crop_area)->int32_t{
            SoftwareFrameWithMemory i420 = {{kSoftwareFormatI420,source.width,source.height}};
            i420.alloc();
            libyuv::SplitUVPlane(source.data[1],source.line_size[1],
                    i420.data[1],i420.line_size[1],
                    i420.data[2],i420.line_size[2],
                    source.width/2,source.height/2);

            libyuv::I420ToI422(source.data[0],source.line_size[0],
                    i420.data[1],i420.line_size[1],
                    i420.data[2],i420.line_size[2],
                    dest.data[0],dest.line_size[0],
                    dest.data[1],dest.line_size[1],
                    dest.data[2],dest.line_size[2],
                    source.width,source.height);
            return  0;
        });
        add_converter(kSoftwareFormatNV21,kSoftwareFormatI422,
                      [](SoftwareFrameWithMemory& source,
                         SoftwareFrameWithMemory& dest,
                         RotationMode rotate,
                         const CropArea& crop_area)->int32_t{
            SoftwareFrameWithMemory i420 = {{kSoftwareFormatI420,source.width,source.height}};
            i420.alloc();
            libyuv::SplitUVPlane(source.data[1],source.line_size[1],
                    i420.data[2],i420.line_size[2],
                    i420.data[1],i420.line_size[1],
                    source.width/2,source.height/2);

            libyuv::I420ToI422(source.data[0],source.line_size[0],
                    i420.data[1],i420.line_size[1],
                    i420.data[2],i420.line_size[2],
                    dest.data[0],dest.line_size[0],
                    dest.data[1],dest.line_size[1],
                    dest.data[2],dest.line_size[2],
                    source.width,source.height);
            return  0;
        });

        add_converter(kSoftwareFormatI422,kSoftwareFormatI422,
                      [](SoftwareFrameWithMemory& source,
                         SoftwareFrameWithMemory& dest,
                         RotationMode rotate,
                         const CropArea& crop_area)->int32_t{
            libyuv::I422Copy(source.data[0],source.line_size[0],
                    source.data[1],source.line_size[1],
                    source.data[2],source.line_size[2],
                    dest.data[0],dest.line_size[0],
                    dest.data[1],dest.line_size[1],
                    dest.data[2],dest.line_size[2],
                    source.width,source.height);
            return  0;
        });

        add_converter(kSoftwareFormatNV16,kSoftwareFormatI422,
                      [](SoftwareFrameWithMemory& source,
                         SoftwareFrameWithMemory& dest,
                         RotationMode rotate,
                         const CropArea& crop_area)->int32_t{
            SoftwareFrameWithMemory i422 = {{kSoftwareFormatI422,source.width,source.height}};
            i422.alloc();
            libyuv::SplitUVPlane(source.data[1],source.line_size[1],
                    i422.data[2],i422.line_size[2],
                    i422.data[1],i422.line_size[1],
                    source.width/2,source.height);
            libyuv::I422Copy(source.data[0],source.line_size[0],
                    source.data[1],source.line_size[1],
                    source.data[2],source.line_size[2],
                    dest.data[0],dest.line_size[0],
                    dest.data[1],dest.line_size[1],
                    dest.data[2],dest.line_size[2],
                    source.width,source.height);
            return  0;
        });

        libyuv::rgb24
    }
    static void add_converter(SoftwareFrameFormat input,SoftwareFrameFormat output,Converter converter){
        uint64_t cookie = input*10000+output;
        converters_[cookie] = converter;
    }
    static Converter get_convertor(SoftwareFrameFormat input,SoftwareFrameFormat output){
        init();

        uint64_t cookie = input*10000+output;
        if(converters_.find(cookie) == converters_.end())
            return Converter();
        return converters_[cookie];
    }
    static std::map<uint64_t,Converter> converters_;
};
std::map<uint64_t,Converter> FixedConverters::converters_;

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

    if(dest_format == kSoftwareFormatI420){

    }
    else if(dest_format == kSoftwareFormatI422){
       if(source_format == kSoftwareFormatI420){

        }
        else if(source_format == kSoftwareFormatYV12){

        }
        else if(source_format == kSoftwareFormatNV12){

        }
        else if(source_format == kSoftwareFormatNV21){

        }
         /*else if(source_format == kSoftwareFormatI422){
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFormatNV16){
            libyuv::SplitUVPlane()
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFormatNV61){
            libyuv::SplitUVPlane()
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFormatYUYV422){
            libyuv::YUY2ToI422()
        }
        else if(source_format == kSoftwareFormatYVYU422){
            libyuv::ARGBToABGR()
            libyuv::YUY2ToI422()
        }
        else if(source_format == kSoftwareFormatUYVY422){
            libyuv::UYVYToI422()
        }
        else if(source_format == kSoftwareFormatI444){
            libyuv::ScalePlane()
            libyuv::ScalePlane()
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFormatNV24){
            libyuv::SplitUVPlane
            libyuv::ScalePlane()
            libyuv::ScalePlane()
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFormatNV42){
            libyuv::SplitUVPlane
            libyuv::ScalePlane()
            libyuv::ScalePlane()
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFormatYUV444){
            libyuv::SplitRGBPlane()
            libyuv::ScalePlane()
            libyuv::ScalePlane()
            libyuv::I422Copy()
        }
        else if(source_format == kSoftwareFormatRGB24){
            libyuv::RAWToARGB()
            libyuv::ARGBToI422()
        }
        else if(source_format == kSoftwareFormatBGR24){
            libyuv::RGB24ToARGB()
            libyuv::ARGBToI422()
        }
        else if(source_format == kSoftwareFormatRGBA32){
            libyuv::ABGRToARGB()
            libyuv::ARGBToI422()
        }
        else if(source_format == kSoftwareFormatBGRA32){
            libyuv::ARGBToI422()
        }
        else if(source_format == kSoftwareFormatARGB32){
            libyuv::BGRAToARGB()
            libyuv::ARGBToI422()
        }
        else if(source_format == kSoftwareFormatABGR32){
            libyuv::RGBAToARGB()
            libyuv::ARGBToI422()
        }
        else if(source_format == kSoftwareFormatGRAY8){
            //new i420 frame
            libyuv::CopyPlane()
        }
        else if(source_format == kSoftwareFormatGRAY8A){
            libyuv::SplitUVPlane()
            //new i420 frame
            libyuv::CopyPlane()
            libyuv::I420ToARGB()
            libyuv::ARGBCopyYToAlpha()
        }*/
    }
    return 0;
}
