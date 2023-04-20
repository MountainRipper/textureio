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
                g_soft_format_names[input],g_soft_format_names[output]);
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

int32_t ConvertManager::crop_rotate_scale(const SoftwareFrame &source, SoftwareFrame &dest, RotationMode rotate, bool crop, bool to_intermediate_only,SamplerMode sampler_mode)
{
    uint32_t width_rotated = source.width;
    uint32_t height_rotated = source.height;
    if(rotate == mr::tio::kRotate90 || rotate == mr::tio::kRotate270){
        width_rotated = source.height;
        height_rotated = source.width;
    }

    FrameArea area_rotated;
    if(crop){
        area_rotated.aspect_crop(width_rotated,
                         height_rotated,
                         dest.width*1.0/dest.height);
    }
    else{
        area_rotated = {0,0,width_rotated,height_rotated};
    }

    FrameArea area_source = area_rotated;
    if(rotate == mr::tio::kRotate90 || rotate == mr::tio::kRotate270){
        std::swap(area_source.x,area_source.y);
        std::swap(area_source.width,area_source.height);
    }

    auto format_map = g_software_format_maps[source.format];
    auto intermediate_format = format_map.intermediate_format;

    SoftwareFrame* croped_rotated_frame = &dest;
    SoftwareFrameWithMemory crop_roate_intermediate_frame;
    bool crop_only = false;
    //do not need scale
    if(area_rotated.width == dest.width && area_rotated.height == dest.height && intermediate_format == dest.format){
        crop_only = true;
    }else{
        crop_roate_intermediate_frame = SoftwareFrameWithMemory(intermediate_format,area_rotated.width,area_rotated.height);
        crop_roate_intermediate_frame.alloc();
        croped_rotated_frame = &crop_roate_intermediate_frame;
    }


    SoftwareFrameWithMemory intermediate_frame = ConvertManager::thread_temporary_frame(intermediate_format,source.width,source.height,101);
    auto convert_to_intermediate = get_convertor(source.format,intermediate_format);
    convert_to_intermediate(source,intermediate_frame);

    if(intermediate_format == kSoftwareFormatI420){
        libyuv::I420Rotate(intermediate_frame.data[0] + (intermediate_frame.line_size[0] * area_source.y + area_source.x), intermediate_frame.line_size[0],
                    intermediate_frame.data[1] + (intermediate_frame.line_size[1] * area_source.y / 2 + area_source.x / 2), intermediate_frame.line_size[1],
                    intermediate_frame.data[2] + (intermediate_frame.line_size[2] * area_source.y / 2 + area_source.x / 2), intermediate_frame.line_size[2],
                    croped_rotated_frame->data[0], croped_rotated_frame->line_size[0],
                    croped_rotated_frame->data[1], croped_rotated_frame->line_size[1],
                    croped_rotated_frame->data[2], croped_rotated_frame->line_size[2],
                    area_source.width, area_source.height,
                    static_cast<libyuv::RotationMode>(rotate));
    }
    if(intermediate_format == kSoftwareFormatI422){
        libyuv::I422Rotate(intermediate_frame.data[0] + (intermediate_frame.line_size[0] * area_source.y + area_source.x), intermediate_frame.line_size[0],
                    intermediate_frame.data[1] + (intermediate_frame.line_size[1] * area_source.y + area_source.x / 2), intermediate_frame.line_size[1],
                    intermediate_frame.data[2] + (intermediate_frame.line_size[2] * area_source.y + area_source.x / 2), intermediate_frame.line_size[2],
                    croped_rotated_frame->data[0], croped_rotated_frame->line_size[0],
                    croped_rotated_frame->data[1], croped_rotated_frame->line_size[1],
                    croped_rotated_frame->data[2], croped_rotated_frame->line_size[2],
                    area_source.width, area_source.height,
                    static_cast<libyuv::RotationMode>(rotate));

    }
    if(intermediate_format == kSoftwareFormatI444){
        libyuv::I444Rotate(intermediate_frame.data[0] + (intermediate_frame.line_size[0] * area_source.y + area_source.x), intermediate_frame.line_size[0],
                    intermediate_frame.data[1] + (intermediate_frame.line_size[1] * area_source.y + area_source.x), intermediate_frame.line_size[1],
                    intermediate_frame.data[2] + (intermediate_frame.line_size[2] * area_source.y + area_source.x), intermediate_frame.line_size[2],
                    croped_rotated_frame->data[0], croped_rotated_frame->line_size[0],
                    croped_rotated_frame->data[1], croped_rotated_frame->line_size[1],
                    croped_rotated_frame->data[2], croped_rotated_frame->line_size[2],
                    area_source.width, area_source.height,
                    static_cast<libyuv::RotationMode>(rotate));
    }
    if(intermediate_format == kSoftwareFormatBGRA32){
        libyuv::ARGBRotate(intermediate_frame.data[0] + (intermediate_frame.line_size[0] * area_source.y + area_source.x * 4), intermediate_frame.line_size[0],
                croped_rotated_frame->data[0], croped_rotated_frame->line_size[0],
                area_source.width, area_source.height,
                static_cast<libyuv::RotationMode>(rotate));
    }

    if(crop_only && to_intermediate_only)
        return 0;

    libyuv::FilterMode filter_map_tio[] = {
        libyuv::kFilterLinear,
        libyuv::kFilterNone,
        libyuv::kFilterLinear,
        libyuv::kFilterBilinear, //just for software scale
        libyuv::kFilterBox, //just for software scale
    };

    SoftwareFrame* scaled_frame = &dest;
    SoftwareFrameWithMemory scale_intermediate_frame;
    bool scale_only = false;
    if(intermediate_format == dest.format || to_intermediate_only){
        scale_only = true;
    }
    else{
        scale_intermediate_frame = SoftwareFrameWithMemory(intermediate_format,dest.width,dest.height);
        scale_intermediate_frame.alloc();
        scaled_frame = &scale_intermediate_frame;
    }
    if(!crop_only){
        //need scale
        if(intermediate_format == kSoftwareFormatI420){
            libyuv::I420Scale(croped_rotated_frame->data[0], croped_rotated_frame->line_size[0],
                    croped_rotated_frame->data[1], croped_rotated_frame->line_size[1],
                    croped_rotated_frame->data[2], croped_rotated_frame->line_size[2],
                    croped_rotated_frame->width,croped_rotated_frame->height,
                    scaled_frame->data[0], scaled_frame->line_size[0],
                    scaled_frame->data[1], scaled_frame->line_size[1],
                    scaled_frame->data[2], scaled_frame->line_size[2],
                    scaled_frame->width,scaled_frame->height,
                    filter_map_tio[sampler_mode]);
        }
        else if(intermediate_format == kSoftwareFormatI422){
            libyuv::I422Scale(croped_rotated_frame->data[0], croped_rotated_frame->line_size[0],
                    croped_rotated_frame->data[1], croped_rotated_frame->line_size[1],
                    croped_rotated_frame->data[2], croped_rotated_frame->line_size[2],
                    croped_rotated_frame->width,croped_rotated_frame->height,
                    scaled_frame->data[0], scaled_frame->line_size[0],
                    scaled_frame->data[1], scaled_frame->line_size[1],
                    scaled_frame->data[2], scaled_frame->line_size[2],
                    scaled_frame->width,scaled_frame->height,
                    filter_map_tio[sampler_mode]);
        }
        else if(intermediate_format == kSoftwareFormatI444){
            libyuv::I444Scale(croped_rotated_frame->data[0], croped_rotated_frame->line_size[0],
                    croped_rotated_frame->data[1], croped_rotated_frame->line_size[1],
                    croped_rotated_frame->data[2], croped_rotated_frame->line_size[2],
                    croped_rotated_frame->width,croped_rotated_frame->height,
                    scaled_frame->data[0], scaled_frame->line_size[0],
                    scaled_frame->data[1], scaled_frame->line_size[1],
                    scaled_frame->data[2], scaled_frame->line_size[2],
                    scaled_frame->width,scaled_frame->height,
                    filter_map_tio[sampler_mode]);
        }
        else if(intermediate_format == kSoftwareFormatBGRA32){
            libyuv::ARGBScale(croped_rotated_frame->data[0], croped_rotated_frame->line_size[0],
                    croped_rotated_frame->width,croped_rotated_frame->height,
                    scaled_frame->data[0], scaled_frame->line_size[0],
                    scaled_frame->width,scaled_frame->height,
                    filter_map_tio[sampler_mode]);
        }
    }

    if(scale_only){
        dest.format = intermediate_format;
        return 0;
    }


    auto convert_to_final = get_convertor(intermediate_format,dest.format);

    convert_to_final(*scaled_frame,dest);

    //
    return 0;
}

SoftwareFrameWithMemory ConvertManager::thread_temporary_frame(SoftwareFrameFormat format, uint32_t width, uint32_t height, uint8_t id)
{
    if(temporary_frames_.find(id) == temporary_frames_.end()){
        temporary_frames_[id] = ThreadTemporaryFrame();
    }
    return temporary_frames_[id].get_temporary_frame(width,height,format);
}
