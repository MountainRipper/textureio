
#include <cassert>
#include <math.h>
#include <tio/tio_software_frame.h>
#include "convert_manager.h"

void SoftwareFrame::clear(mr::tio::FrameArea area){
    if(area.width == 0 || area.height == 0)
        area = FrameArea{0,0,width,height};

    auto& planes = g_software_format_info[format].planes;

    if(format <= kSoftwareFormatYUVEnd){
        if(format == kSoftwareFormatYUYV422 || format == kSoftwareFormatYVYU422 || format == kSoftwareFormatUYVY422){

        }
        else if(format == kSoftwareFormatYUV444){

        }
        else{
            for(int index = 0; index < 4; index++){
                auto& plane = planes[index];
                if(plane.channels == 0)
                    break;
                if(index == 0){
                    //Y plane
                    libyuv::SetPlane(data[0] + line_size[0] * area.y + area.x,
                            line_size[0],
                            area.width,
                            area.height,
                            0);
                }
                else{
                    libyuv::SetPlane(data[index] + (line_size[index] * int(area.y * plane.scale_y)) + (int(area.x * plane.scale_x) * plane.channels),
                            line_size[index],
                            area.width * plane.scale_x * plane.channels,
                            area.height * plane.scale_y,
                            128);
                }
            }
        }
    }
    else{
        libyuv::SetPlane(data[0] + line_size[0] * area.y + area.x * planes[0].channels,
                line_size[0],
                area.width * planes[0].channels,
                area.height,
                0);
    }

}
SoftwareFrameWithMemory::SoftwareFrameWithMemory(const SoftwareFrameWithMemory &other){
    memcpy(this,&other,sizeof(SoftwareFrame));
    for(int index = 0; index < 4 ; index++)
        plane_size[index] = other.plane_size[index];
    data_buffer_ = other.data_buffer_;
    frame_memory_ = other.frame_memory_;
}

SoftwareFrameWithMemory::SoftwareFrameWithMemory(SoftwareFrameFormat format, uint32_t width, uint32_t height){
    this->format = format;
    this->width = width;
    this->height = height;
}

SoftwareFrameWithMemory::SoftwareFrameWithMemory(SoftwareFrameFormat format, uint32_t width, uint32_t height, uint8_t *data){
    this->format = format;
    this->width = width;
    this->height = height;
    attach(data);
}

void SoftwareFrameWithMemory::alloc(){
    uint32_t width_adjust = (width + 1) & ~1;
    uint32_t bpp = g_software_format_info[format].bpp;
    uint32_t bytes = width_adjust*height*bpp/8;
    frame_memory_ = std::shared_ptr<uint8_t>(new uint8_t[bytes],std::default_delete<uint8_t[]>());
    data_buffer_ = frame_memory_.get();
    fill_plane();
}

void SoftwareFrameWithMemory::attach(uint8_t *data){
    data_buffer_ = data;
    fill_plane();
}

void SoftwareFrameWithMemory::clone_from(const SoftwareFrameWithMemory &source){
    this->format = format;
    this->width = width;
    this->height = height;
    alloc();

    uint8_t depth = 8;

    auto copy_converter = ConvertManager::get_convertor(format,format);
    copy_converter(source,*static_cast<SoftwareFrame*>(this));
}

SoftwareFrameWithMemory SoftwareFrameWithMemory::clone_new()
{
    SoftwareFrameWithMemory new_frame = *this;
    new_frame.clone_from(*this);
    return new_frame;
}

void SoftwareFrameWithMemory::fill_plane(uint8_t *data_from){
    uint8_t depth = 8;
    line_size[0] = line_size[1] = line_size[2] = line_size[3] = 0;
    data[0] = data[1] = data[2] = data[3] = nullptr;

    uint8_t* plane_ptr = data_buffer_;
    auto& planes = g_software_format_info[format].planes;

    for(int index = 0; index < 4; index++){
        auto& plane = planes[index];
        if(plane.channels == 0)
            break;

        uint32_t plane_width = (width * plane.scale_x);
        uint8_t align_bytes = 1.0 / plane.scale_x;
        if((plane.scale_x != 1) && (width % align_bytes != 0)){
            plane_width = (plane_width + align_bytes - 1) / align_bytes * align_bytes;
        }
        uint32_t plane_height = (height * plane.scale_y);
        line_size[index] = plane_width * plane.channels * depth / 8;
        plane_size[index] = plane_height * line_size[index];
        data[index] = plane_ptr;
        plane_ptr += plane_size[index];
    }
}

SoftwareFrameConvert::SoftwareFrameConvert()
{
    ConvertManager::init();
}

int32_t SoftwareFrameConvert::convert(const SoftwareFrame &source, SoftwareFrame &dest, RotationMode rotate, mr::tio::FillMode fill_mode)
{
    auto source_format = source.format;
    auto dest_format = dest.format;

    if(rotate == kRotate0 && source.width == dest.width && source.height == dest.height){
        auto converter = ConvertManager::get_convertor(source_format,dest_format);

        if(converter){
            fprintf(stderr,"Direct Converter from %s to %s.\n",g_software_format_info[source_format].name,g_software_format_info[dest_format].name);
            converter(source,dest);
        }
        else{
            SoftwareFrameFormat intermediate_format = g_software_format_maps[source_format].intermediate_format;

            auto converter_to_intermediate = ConvertManager::get_convertor(source_format,intermediate_format);
            auto converter_from_intermediate = ConvertManager::get_convertor(intermediate_format,dest_format);

            assert(converter_to_intermediate);
            assert(converter_from_intermediate);

            fprintf(stderr,"Indirect Converter from %s to %s,then to %s.\n",
                    g_software_format_info[source_format].name,
                    g_software_format_info[intermediate_format].name,
                    g_software_format_info[dest_format].name);

            SoftwareFrameWithMemory intermediate_frame = {intermediate_format,source.width,source.height};
            intermediate_frame = ConvertManager::thread_temporary_frame(intermediate_format,source.width,source.height,'c'+1);
            converter_to_intermediate(source,intermediate_frame);
            converter_from_intermediate(intermediate_frame,dest);
        }
    }
    else{
        ConvertManager::crop_rotate_scale(source,dest,rotate,fill_mode,kSamplerBilinear);
    }
    return 0;
}
