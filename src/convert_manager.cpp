#include "convert_manager.h"
#include "convert_to_i420.h"
#include "convert_to_i422.h"
#include "convert_to_i444.h"
#include "convert_to_bgra.h"
#include "convert_from_i420.h"
#include "convert_from_i422.h"
#include "convert_from_i444.h"
#include "convert_from_bgra.h"
#include "convert_direct.h"
#include <thread>

#define MAKE_CONVERTER_COOKIE(from,to) (from*10000+to)

std::map<uint32_t,Converter> ConvertManager::converters_;
std::set<uint32_t> ConvertManager::slower_converters_;
std::map<uint8_t,ThreadTemporaryFrame> ConvertManager::temporary_frames_;

libyuv::FilterMode filter_map_tio[] = {
    libyuv::kFilterLinear,
    libyuv::kFilterNone,
    libyuv::kFilterLinear,
    libyuv::kFilterBilinear, //just for software scale
    libyuv::kFilterBox, //just for software scale
};

void ConvertManager::init(){
    static bool initialized = false;
    if(initialized)
        return;
    initialized = true;

    ConvertDirect::register_copy_converter();

    ConvertToI420::register_converter();
    ConvertToI422::register_converter();
    ConvertToI444::register_converter();
    ConvertToBGRA::register_converter();

    ConvertFromI420::register_converter();
    ConvertFromI422::register_converter();
    ConvertFromI444::register_converter();
    ConvertFromBGRA::register_converter();

    ConvertDirect::register_direct_converter();
    fprintf(stderr,"totle converters:%zu\n",converters_.size());
}

void ConvertManager::add_converter(SoftwareFrameFormat input, SoftwareFrameFormat output, Converter converter){
    if( input >= kSoftwareFormatCount || output >= kSoftwareFormatCount)
        return;

    uint32_t cookie = MAKE_CONVERTER_COOKIE(input,output);
    if(converters_.find(cookie) != converters_.end()){
        fprintf(stderr,"Warnning: Converter from %s to %s already exsit, overwrite old.\n",
                g_software_format_info[input].name,g_software_format_info[output].name);
    }
    converters_[cookie] = converter;
}

Converter ConvertManager::get_convertor(SoftwareFrameFormat input, SoftwareFrameFormat output){
    init();

    uint32_t cookie = MAKE_CONVERTER_COOKIE(input,output);
    if(converters_.find(cookie) == converters_.end())
        return Converter();
    return converters_[cookie];
}

void ConvertManager::mark_slower_convertor(SoftwareFrameFormat input, SoftwareFrameFormat output)
{
    uint32_t cookie = MAKE_CONVERTER_COOKIE(input,output);
    slower_converters_.insert(cookie);
}

bool ConvertManager::is_slower_convertor(SoftwareFrameFormat input, SoftwareFrameFormat output)
{
    uint32_t cookie = MAKE_CONVERTER_COOKIE(input,output);
    if(slower_converters_.find(cookie) != slower_converters_.end())
        return true;
    return false;
}

int32_t ConvertManager::crop_rotate_scale(const SoftwareFrame &source, SoftwareFrame &dest, RotationMode rotate_mode, FillMode fill_mode,SamplerMode sampler_mode)
{
    //when crop, 1:to intermediate, 2:rotate/crop frame to dest rotation and aspect ratio, 3:scale to dest size, 4: convert to dest format
    //when wrap, 1:to intermediate, 2:rotate whole frame to dest rotation(temp intermediate frame), 3:scale to dest size/area,  4: convert to dest format
    //when fill, 1:to intermediate, 3:rotate whole frame to dest rotation(temp intermediate frame), 3:scale to dest size, 4: convert to dest format

    bool source_is_intermediate = IS_INTERMEDIATE_FORMAT(source.format);
    auto format_map = g_software_format_maps[source.format];
    auto intermediate_format = format_map.intermediate_format;

    const SoftwareFrame* intermediate_frame_ptr = &source;
    SoftwareFrameWithMemory intermediate_frame;
    if(!source_is_intermediate){
        intermediate_frame = ConvertManager::thread_temporary_frame(intermediate_format,source.width,source.height,'c'+1);
        auto convert_to_intermediate = get_convertor(source.format,intermediate_format);
        convert_to_intermediate(source,intermediate_frame);
        intermediate_frame_ptr = &intermediate_frame;
    }

    uint32_t width_rotated = source.width;
    uint32_t height_rotated = source.height;
    if(rotate_mode == mr::tio::kRotate90 || rotate_mode == mr::tio::kRotate270){
        width_rotated = source.height;
        height_rotated = source.width;
    }

    FrameArea area_source;
    FrameArea area_after_rotate;
    FrameArea area_dest;
    if(fill_mode == kAspectCrop){
        area_source.aspect_crop(width_rotated,
                         height_rotated,
                         dest.width*1.0/dest.height);
        area_after_rotate = {0,0,area_source.width,area_source.height};
        area_dest = {0,0,dest.width,dest.height};

        if(rotate_mode == mr::tio::kRotate90 || rotate_mode == mr::tio::kRotate270){
            //aspect_crop is calc with rotated w/h,need swap back
            std::swap(area_source.x,area_source.y);
            std::swap(area_source.width,area_source.height);
        }
    }
    else if(fill_mode == kAspectFit){
        area_dest.aspect_crop(dest.width,
                         dest.height,
                         width_rotated*1.0/height_rotated);

        area_source = {0,0,source.width,source.height};
        area_after_rotate = {0,0,width_rotated,height_rotated};
    }
    else if(fill_mode == kStretchFill){
        area_source = {0,0,source.width,source.height};
        area_after_rotate = {0,0,width_rotated,height_rotated};
        area_dest = {0,0,dest.width,dest.height};
    }

    SoftwareFrame* rotated_frame_ptr = nullptr;
    SoftwareFrameWithMemory rotated_frame;
    bool rotate_only = false;
    if(area_after_rotate.width == dest.width && area_after_rotate.height == dest.height && dest.format == intermediate_format){
        rotate_only = true;
        rotated_frame_ptr = &dest;
    }
    else{
        rotated_frame = ConvertManager::thread_temporary_frame(intermediate_format,area_after_rotate.width,area_after_rotate.height,'c'+2);
        rotated_frame_ptr = &rotated_frame;
    }

    rotate(*intermediate_frame_ptr,*rotated_frame_ptr,area_source,area_after_rotate,rotate_mode);

    if(rotate_only)
        return 0;


    SoftwareFrame* scaled_frame_ptr = nullptr;
    SoftwareFrameWithMemory scaled_frame;
    bool scale_only = false;
    if(intermediate_format == dest.format){
        scale_only = true;
        scaled_frame_ptr = &dest;
    }
    else{
        //temporary frame 1 is not in use anymore
        //make a temporary frame for scale to dest size
        scaled_frame = ConvertManager::thread_temporary_frame(intermediate_format,dest.width,dest.height,'c'+1);
        scaled_frame_ptr = &scaled_frame;
    }

    scale(*rotated_frame_ptr,*scaled_frame_ptr,area_after_rotate,area_dest,sampler_mode);

    //now size if same as dest, only format need convert
    if(scale_only)
        return 0;

    auto convert_to_final = get_convertor(intermediate_format,dest.format);

    convert_to_final(*scaled_frame_ptr,dest);

    return 0;
}

int32_t ConvertManager::scale(const SoftwareFrame &source, SoftwareFrame &dest,FrameArea area_source,  FrameArea area_dest, SamplerMode sampler_mode)
{
    SoftwareFrameFormat format = source.format;
    if(format != dest.format)
        return kErrorFormatNotMatch;

    if(area_source.width == 0 || area_dest.height == 0)
        area_source = FrameArea{0,0,source.width,source.height};

    if(area_dest.width == 0 || area_dest.height == 0)
        area_dest = FrameArea{0,0,dest.width,dest.height};

    if(format == kSoftwareFormatI420){
        libyuv::I420Scale(source.data[0] + (source.linesize[0] * area_source.y + area_source.x), source.linesize[0],
                source.data[1] + (source.linesize[1] * (area_source.y >> 1) + (area_source.x >> 1)), source.linesize[1],
                source.data[2] + (source.linesize[2] * (area_source.y >> 1) + (area_source.x >> 1)), source.linesize[2],
                area_source.width,area_source.height,
                dest.data[0] + (dest.linesize[0] * area_dest.y + area_dest.x), dest.linesize[0],
                dest.data[1] + (dest.linesize[1] * (area_dest.y >> 1) + (area_dest.x >> 1)), dest.linesize[1],
                dest.data[2] + (dest.linesize[2] * (area_dest.y >> 1) + (area_dest.x >> 1)), dest.linesize[2],
                area_dest.width,area_dest.height,
                filter_map_tio[sampler_mode]);
    }
    else if(format == kSoftwareFormatI422){
        libyuv::I422Scale(source.data[0] + (source.linesize[0] * area_source.y + area_source.x), source.linesize[0],
                source.data[1] + (source.linesize[1] * area_source.y + (area_source.x >> 1)), source.linesize[1],
                source.data[2] + (source.linesize[2] * area_source.y + (area_source.x >> 1)), source.linesize[2],
                area_source.width,area_source.height,
                dest.data[0] + (dest.linesize[0] * area_dest.y + area_dest.x), dest.linesize[0],
                dest.data[1] + (dest.linesize[1] * area_dest.y + (area_dest.x >> 1)), dest.linesize[1],
                dest.data[2] + (dest.linesize[2] * area_dest.y + (area_dest.x >> 1)), dest.linesize[2],
                area_dest.width,area_dest.height,
                filter_map_tio[sampler_mode]);
    }
    else if(format == kSoftwareFormatI444){
        libyuv::I444Scale(source.data[0] + (source.linesize[0] * area_source.y + area_source.x), source.linesize[0],
                source.data[1] + (source.linesize[1] * area_source.y + area_source.x), source.linesize[1],
                source.data[2] + (source.linesize[2] * area_source.y + area_source.x), source.linesize[2],
                area_source.width,area_source.height,
                dest.data[0] + (dest.linesize[0] * area_dest.y + area_dest.x), dest.linesize[0],
                dest.data[1] + (dest.linesize[1] * area_dest.y + area_dest.x), dest.linesize[1],
                dest.data[2] + (dest.linesize[2] * area_dest.y + area_dest.x), dest.linesize[2],
                area_dest.width,area_dest.height,
                filter_map_tio[sampler_mode]);
    }
    else if(format == kSoftwareFormatBGRA32){
        libyuv::ARGBScale(source.data[0] + (source.linesize[0] * area_source.y + area_source.x * 4), source.linesize[0],
                area_source.width,area_source.height,
                dest.data[0] + (dest.linesize[0] * area_dest.y + area_dest.x * 4), dest.linesize[0],
                area_dest.width,area_dest.height,
                filter_map_tio[sampler_mode]);        
    }
    else{
        return kErrorFormatNotSupport;
    }

    if(area_dest.x == 0 && area_dest.height < dest.height){
        dest.clear(FrameArea{0, 0, dest.width, area_dest.y});
        dest.clear(FrameArea{0, area_dest.y+area_dest.height,dest.width, dest.height-area_dest.y-area_dest.height});
    }
    else if(area_dest.width < dest.width){
        dest.clear(FrameArea{0, 0, area_dest.x, dest.height});
        dest.clear(FrameArea{area_dest.x + area_dest.width, 0, dest.width - area_dest.x - area_dest.width, dest.height});
    }
    return 0;
}

int32_t ConvertManager::rotate(const SoftwareFrame &source, SoftwareFrame &dest, FrameArea area_source, FrameArea area_dest, RotationMode rotate)
{
    SoftwareFrameFormat format = source.format;
    if(format != dest.format)
        return kErrorFormatNotMatch;

    if(area_source.width == 0 || area_dest.height == 0)
        area_source = FrameArea{0,0,source.width,source.height};

    if(area_dest.width == 0 || area_dest.height == 0)
        area_dest = FrameArea{0,0,dest.width,dest.height};

    if(rotate == kRotate0 || rotate == kRotate180){
        if(area_source.width != area_dest.width || area_source.height != area_dest.height)
            return kErrorFormatNotMatch;
    }
    else{
        if(area_source.width != area_dest.height || area_source.height != area_dest.width)
            return kErrorFormatNotMatch;
    }

    if(format == kSoftwareFormatI420){
        libyuv::I420Rotate(source.data[0] + (source.linesize[0] * area_source.y + area_source.x), source.linesize[0],
                source.data[1] + (source.linesize[1] * (area_source.y >> 1) + (area_source.x >> 1)), source.linesize[1],
                source.data[2] + (source.linesize[2] *(area_source.y >> 1)+ (area_source.x >> 1)), source.linesize[2],
                dest.data[0] + (dest.linesize[0] * area_dest.y + area_dest.x), dest.linesize[0],
                dest.data[1] + (dest.linesize[1] * (area_dest.y >> 1) + (area_dest.x >> 1)), dest.linesize[1],
                dest.data[2] + (dest.linesize[2] * (area_dest.y >> 1) + (area_dest.x >> 1)), dest.linesize[2],
                    area_source.width, area_source.height,
                    static_cast<libyuv::RotationMode>(rotate));
    }
    else if(format == kSoftwareFormatI422){
        libyuv::I422Rotate(source.data[0] + (source.linesize[0] * area_source.y + area_source.x), source.linesize[0],
                source.data[1] + (source.linesize[1] * area_source.y + (area_source.x >> 1)), source.linesize[1],
                source.data[2] + (source.linesize[2] * area_source.y + (area_source.x >> 1)), source.linesize[2],
                dest.data[0] + (dest.linesize[0] * area_dest.y + area_dest.x), dest.linesize[0],
                dest.data[1] + (dest.linesize[1] * area_dest.y + (area_dest.x >> 1)), dest.linesize[1],
                dest.data[2] + (dest.linesize[2] * area_dest.y + (area_dest.x >> 1)), dest.linesize[2],
                    area_source.width, area_source.height,
                    static_cast<libyuv::RotationMode>(rotate));

    }
    else if(format == kSoftwareFormatI444){
        libyuv::I444Rotate(source.data[0] + (source.linesize[0] * area_source.y + area_source.x), source.linesize[0],
                source.data[1] + (source.linesize[1] * area_source.y + area_source.x), source.linesize[1],
                source.data[2] + (source.linesize[2] * area_source.y + area_source.x), source.linesize[2],
                dest.data[0] + (dest.linesize[0] * area_dest.y + area_dest.x), dest.linesize[0],
                dest.data[1] + (dest.linesize[1] * area_dest.y + area_dest.x), dest.linesize[1],
                dest.data[2] + (dest.linesize[2] * area_dest.y + area_dest.x), dest.linesize[2],
                    area_source.width, area_source.height,
                    static_cast<libyuv::RotationMode>(rotate));
    }
    else if(format == kSoftwareFormatBGRA32){
        libyuv::ARGBRotate(source.data[0] + (source.linesize[0] * area_source.y + area_source.x * 4), source.linesize[0],
                dest.data[0] + (dest.linesize[0] * area_dest.y + area_dest.x * 4), dest.linesize[0],
                    area_source.width, area_source.height,
                    static_cast<libyuv::RotationMode>(rotate));
    }
    else{
        return kErrorFormatNotSupport;
    }

    return 0;
}

SoftwareFrameWithMemory ConvertManager::thread_temporary_frame(SoftwareFrameFormat format, uint32_t width, uint32_t height, uint8_t id)
{
    SoftwareFrameWithMemory frame = SoftwareFrameWithMemory(format,width,height);
    frame.alloc();
    return frame;
//    if(temporary_frames_.find(id) == temporary_frames_.end()){
//        temporary_frames_[id] = ThreadTemporaryFrame();
//    }
//    return temporary_frames_[id].get_temporary_frame(width,height,format);
}
