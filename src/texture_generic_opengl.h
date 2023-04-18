#ifndef TEXTUREGENERICOPENGL_H
#define TEXTUREGENERICOPENGL_H

#include <string>
#include <vector>
#include <tio/tio_types.h>

namespace mr {
namespace tio {

class TextureGenericOpenGL
{
public:
    TextureGenericOpenGL();

    static int32_t upload(const SoftwareFrame& frame, GraphicTexture& texture,SamplerMode sampler_mode = kSamplerAuto);
    static uint64_t create_texture(const SoftwareFrame &frame, GraphicTexture &texture, SamplerMode sampler_mode = kSamplerAuto);
    static int32_t release_texture(uint64_t texture_id);
    static std::string reference_shader_software(SoftwareFrameFormat format, YuvColorSpace color_space);
private:
    static void get_capability();
private:
    static bool  gles_;
    static float version_;
    static bool  bgra_support_;
    static std::vector<std::string> extensions_;
};


}//namespace tio
}//namespace mountain-ripper

#endif // TEXTUREGENERICOPENGL_H
