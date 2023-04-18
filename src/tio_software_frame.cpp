
#include <cassert>
#include <math.h>
#include <tio/tio_software_frame.h>
#include "convert_manager.h"


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
    uint32_t width_adjust = (width + 1) / 2 * 2;
    uint32_t bpp = g_soft_format_bpps[format];
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
    auto& planes = g_software_format_planers[format].planes;

    if(format == kSoftwareFormatNV16)
        printf("%p\n",this);
    for(int index = 0; index < 4; index++){
        auto& plane = planes[index];
        if(plane.channels == 0)
            break;

        uint32_t plane_width = (width * plane.scale_x);
        if((index != 0) && (plane_width%2 != 0))
            plane_width += 1;
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

int32_t SoftwareFrameConvert::convert(const SoftwareFrame &source, SoftwareFrame &dest,RotationMode rotate,const CropArea& crop_area)
{
    auto source_format = source.format;
    auto dest_format = dest.format;
    int32_t crop_width = (crop_area.crop_width == UINT32_MAX)?source.width:crop_area.crop_width;
    int32_t crop_height = (crop_area.crop_height == UINT32_MAX)?source.height:crop_area.crop_height;

    auto converter = ConvertManager::get_convertor(source_format,dest_format);

    if(converter){
        fprintf(stderr,"Direct Converter from %s to %s.\n",g_soft_format_names[source_format],g_soft_format_names[dest_format]);
        converter(source,dest);
    }
    else{
        SoftwareFrameFormat intermediate_format = g_software_format_maps[source_format].intermediate_format;

        auto converter_to_intermediate = ConvertManager::get_convertor(source_format,intermediate_format);
        auto converter_from_intermediate = ConvertManager::get_convertor(intermediate_format,dest_format);

        assert(converter_to_intermediate);
        assert(converter_from_intermediate);

        fprintf(stderr,"Indirect Converter from %s to %s,then to %s.\n",g_soft_format_names[source_format],g_soft_format_names[intermediate_format],g_soft_format_names[dest_format]);

        SoftwareFrameWithMemory intermediate_frame = {intermediate_format,source.width,source.height};
        intermediate_frame = ConvertManager::thread_temporary_frame(intermediate_format,source.width,source.height,100);
        converter_to_intermediate(source,intermediate_frame);
        converter_from_intermediate(intermediate_frame,dest);
    }
    return 0;
}
