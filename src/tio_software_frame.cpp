
#include <cassert>
#include <tio/tio_software_frame.h>
#include "convert_manager.h"
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
        fprintf(stderr,"Direct Converter from %s to %s.\n",
                g_soft_format_names[source_format],g_soft_format_names[dest_format]);
        converter(source,dest,rotate,crop_area);
    }
    else{
        SoftwareFrameFormat intermediate_format = g_software_format_maps[source_format].intermediate_format;

        auto converter_to_intermediate = ConvertManager::get_convertor(source_format,intermediate_format);
        auto converter_from_intermediate = ConvertManager::get_convertor(intermediate_format,dest_format);

        assert(converter_to_intermediate);
        assert(converter_from_intermediate);

        fprintf(stderr,"Indirect Converter from %s to %s,then to %s.\n",
                g_soft_format_names[source_format],g_soft_format_names[intermediate_format],g_soft_format_names[dest_format]);

        SoftwareFrameWithMemory intermediate_frame = {intermediate_format,source.width,source.height};
        intermediate_frame.alloc();
        converter_to_intermediate(source,intermediate_frame,rotate,crop_area);
        converter_from_intermediate(intermediate_frame,dest,kRotate0,CropArea());
    }
    return 0;
}
