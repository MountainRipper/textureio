#ifndef MP_VIDEO2HW_H_
#define MP_VIDEO2HW_H_
#include "tio_types.h"
namespace mr {
namespace tio {


class TextureIO
{
public:
    TextureIO();

    static int32_t software_frame_to_graphic(const SoftwareFrame& frame,GraphicTexture& texture,SamplerMode sampler_mode = kSamplerAuto);
    static int32_t software_frame_to_hardware_frame();
    static int32_t hardware_frame_to_graphic();
    static int32_t hardware_frame_to_software_frame();
    static int32_t graphic_to_software_frame();
    static int32_t graphic_to_hardware_frame();

    static const SoftwareFormatPlaner* planers_of_software_frame(SoftwareFrameFormat format);

    static const std::string reference_shader_software(GraphicApi api,SoftwareFrameFormat format,YuvColorSpace color_space = kColorSpaceBt601,float version = 0);
    static const char* reference_shader_hardware(GraphicApi api,HardwareFrameFormat format,float version);
};


}//namespace tio
}//namespace mountain-ripper

#endif // VIDEO2HW_H
