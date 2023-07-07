#include <filesystem>
#include <map>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stb/stb_image.h>
#include <tio/tio_hardware_graphic.h>
#include "texture_generic_opengl.h"

using namespace mr::tio;

ReferenceShader::~ReferenceShader()
{

}

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
    return &g_software_format_info[format];
}

const std::shared_ptr<ReferenceShader> TextureIO::create_reference_shader(GraphicApi api, SoftwareFrameFormat format, YuvColorSpace color_space, float version)
{
    if(api == kGraphicApiOpenGL){
        return  TextureGenericOpenGL::create_reference_shader(format,color_space);
    }

    return std::shared_ptr<ReferenceShader>();
}

int32_t TextureIO::create_texture(const std::string& image,
                                   TextureCreated callback,
                                   SamplerMode sampler_mode)
{
    std::filesystem::path image_path = image;
    std::string ext = image_path.extension().string();

    auto loader = [](std::filesystem::path& image_path,std::map<std::string,FrameArea>& areas,TextureCreated& callback) -> int32_t{
        int pic_width = 0;
        int pic_height = 0;
        int channels = 4;//as rgba
        auto image_data = stbi_load(image_path.string().c_str(),&pic_width,&pic_height,&channels,channels);
        if(image_data == nullptr || channels != 4)
            return kErrorInvalidFrame;

        SoftwareFrameWithMemory frame(g_software_format_of_channel[channels],pic_width,pic_height);
        frame.attach(image_data);

        int ret = 0;
        GraphicTexture texture;
        texture.api = kGraphicApiOpenGL;
        if(texture.api == kGraphicApiOpenGL){
            auto texture_id = TextureGenericOpenGL::create_texture(frame,texture);
        }
        stbi_image_free(image_data);

        FrameArea area = {0,0,(uint32_t)pic_width,(uint32_t)pic_height};
        areas[image_path.string()] = area;
        callback(image_path.string(),texture,areas);
        return 0;
    };

    if(image_path.extension() == ".json"){

        try{
            std::ifstream ifs(image_path.string());
            nlohmann::json image_menifest = nlohmann::json::parse(ifs);
            for(const auto &image_item : image_menifest["images"]){
                int pic_width = image_item["imageWidth"].get<int>();
                int pic_height = image_item["imageHeight"].get<int>();
                if(pic_width == 0 || pic_height == 0)
                    return kErrorInvalidFrame;

                auto image_file = image_path.parent_path() / image_item.value("imageFile",std::string());

                std::map<std::string,FrameArea> areas;
                for(auto& item : image_item["imageAreas"].items()){
                    FrameArea area;
                    area.x = item.value()["x"].get<int>();
                    area.y = item.value()["y"].get<int>();
                    area.width = item.value()["w"].get<int>();
                    area.height = item.value()["h"].get<int>();
                    std::string image_name = image+":"+item.key();
                    areas[image_name] = area;
                }
                loader(image_file,areas,callback);
            }
        }
        catch(...){
            //nlohmann parse exceptions
        }
    }
    else{
        std::map<std::string,FrameArea> areas;
        loader(image_path,areas,callback);
    }

    return 0;
}

int32_t TextureIO::create_texture(const SoftwareFrame &frame, GraphicTexture &texture, SamplerMode sampler_mode)
{
    if(texture.api == kGraphicApiOpenGL){
        return TextureGenericOpenGL::create_texture(frame,texture,sampler_mode);
    }
    return kErrorFormatNotSupport;
}

int32_t TextureIO::release_texture(GraphicTexture &texture)
{
    for(int index = 0; index < 4; index++){
        if(texture.api == kGraphicApiOpenGL){
            if(texture.context[index] > 0){
                TextureGenericOpenGL::release_texture(texture.context[index]);
            }
        }
    }
    return 0;
}

int32_t TextureIO::release_texture(GraphicApi api,uint64_t texture_id)
{
    if(texture_id == kGraphicApiOpenGL){
        return TextureGenericOpenGL::release_texture(texture_id);
    }
    return kErrorInvalidGraphicApi;
}
