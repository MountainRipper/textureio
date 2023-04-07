
#include <tio/tio_software_frame.h>
#include "convert_manager.h"
SoftwareFrameConvert::SoftwareFrameConvert()
{
    ConvertManager::init();
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

    }
    /*
    else if(source_format == kSoftwareFormatGRAY8A){
        GRAY8A to argb
        libyuv::SplitUVPlane()
        //new i420 frame
        libyuv::CopyPlane()
        libyuv::I420ToARGB()
        libyuv::ARGBCopyYToAlpha()
    }*/
    return 0;
}
