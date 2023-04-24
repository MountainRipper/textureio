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
    static std::shared_ptr<ReferenceShader> create_reference_shader(SoftwareFrameFormat format, YuvColorSpace color_space);
    static std::string reference_fragment(SoftwareFrameFormat format, YuvColorSpace color_space);
private:
    static void get_capability();
    static void upload_to_texture(int texture_id,
                                  int texture_unit,
                                  int channels,
                                  int filter,
                                  int gl_format,
                                  uint8_t* data,
                                  int width,
                                  int height,
                                  int linesize);
public:
    static bool  gles_;
    static float version_;
    static bool  bgra_support_;
    static std::vector<std::string> extensions_;
};


}//namespace tio
}//namespace mountain-ripper

#endif // TEXTUREGENERICOPENGL_H
