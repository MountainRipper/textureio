#include <map>
#include <tio/tio_hardware_graphic.h>
#include "texture_generic_opengl.h"

using namespace mr::tio;

TextureIO::TextureIO()
{
}

int32_t TextureIO::software_frame_to_graphic(const SoftwareFrame &frame, GraphicTexture &texture)
{
    if(texture.api == kGraphicApiOpenGL){
        TextureGenericOpenGL::upload(frame,texture);
    }
    return 0;
}

const SoftwareFormatPlaner *TextureIO::planers_of_software_frame(SoftwareFrameFormat format)
{    
    return &f_software_format_planers[format];
}

const std::string TextureIO::reference_shader_software(GraphicApi api, SoftwareFrameFormat format, YuvColorSpace color_space, float version)
{
    std::string ret;
    if(api == kGraphicApiOpenGL){
        ret = TextureGenericOpenGL::reference_shader_software(format,color_space);
    }

    return ret;
}
