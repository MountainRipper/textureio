#include <filesystem>
#include <map>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stb/stb_image.h>
#include <tio/tio_hardware_graphic.h>
#include "texture_generic_opengl.h"

using namespace mr::tio;

TextureIO::TextureIO()
{
}

int32_t TextureIO::software_frame_to_graphic(const SoftwareFrame &frame, GraphicTexture &texture,SamplerMode sampler_mode)
{
    if(texture.api == kGraphicApiOpenGL){
        TextureGenericOpenGL::upload(frame,texture,sampler_mode);
    }
    return 0;
}

const SoftwareFormatPlaner *TextureIO::planers_of_software_frame(SoftwareFrameFormat format)
{    
    return &g_software_format_planers[format];
}

const std::string TextureIO::reference_shader_software(GraphicApi api, SoftwareFrameFormat format, YuvColorSpace color_space, float version)
{
    std::string ret;
    if(api == kGraphicApiOpenGL){
        ret = TextureGenericOpenGL::reference_shader_software(format,color_space);
    }

    return ret;
}

int32_t TextureIO::create_texture(const std::string& image,
                                   GraphicTexture &texture,
                                   std::map<std::string,CropArea>& areas,
                                   SamplerMode sampler_mode)
{
    std::filesystem::path image_path = image;
    std::string ext = image_path.extension();
    if(image_path.extension() == ".json"){

        std::ifstream ifs(image_path.string());
        nlohmann::json image_menifest = nlohmann::json::parse(ifs);
        int pic_width = image_menifest["imageWidth"].get<int>();
        int pic_height = image_menifest["imageHeight"].get<int>();
        if(pic_width == 0 || pic_height == 0)
            return kErrorInvalidFrame;

        image_path = image_path.parent_path() / image_menifest["imageFile"].get<std::string>();

        for(auto& item : image_menifest["imageAreas"].items()){
            CropArea area;
            area.crop_x = item.value()["x"].get<int>();
            area.crop_y = item.value()["y"].get<int>();
            area.crop_width = item.value()["w"].get<int>();
            area.crop_height = item.value()["h"].get<int>();
            areas[item.key()] = area;
        }
    }

    int pic_width = 0;
    int pic_height = 0;
    int channels = 4;//as rgb
    auto image_data = stbi_load(image_path.c_str(),&pic_width,&pic_height,&channels,channels);
    if(image_data == nullptr)
        return kErrorInvalidFrame;

    SoftwareFrameFormat formats_of_channel[] = {kSoftwareFormatNone,kSoftwareFormatGRAY8,kSoftwareFormatGRAY8A,kSoftwareFormatRGB24,kSoftwareFormatRGBA32};

    SoftwareFrameWithMemory frame(formats_of_channel[channels],pic_width,pic_height);
    frame.attach(image_data);

    int ret = 0;
    if(texture.api == kGraphicApiOpenGL){
        auto texture_id = TextureGenericOpenGL::create_texture(frame,texture);
    }
    stbi_image_free(image_data);

    CropArea area = {0,0,(uint32_t)pic_width,(uint32_t)pic_height};
    areas[image_path] = area;

    return ret;
}

int32_t TextureIO::release_texture(GraphicApi api, uint64_t texture_id)
{
    if(api == kGraphicApiOpenGL){
        return TextureGenericOpenGL::release_texture(texture_id);
    }
    return 0;
}
