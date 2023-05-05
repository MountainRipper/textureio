#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_rect_pack.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <tio/tio_software_frame.h>

#include <cxxopts.hpp>

using namespace std;
using namespace mr::tio;

struct Image{
    Image(filesystem::path file_path){
        this->file_path = file_path;
    }
    int32_t load(){
        auto image_buffer = stbi_load(file_path.string().c_str(),&width,&height,&channels,4);
        if(channels == 0){
            fprintf(stderr,"image:%s open failed\n",file_path.string().c_str());
            return -1;
        }

        image.clone_from(SoftwareFrameWithMemory(kSoftwareFormatRGBA32,width,height).attach(image_buffer));

        stbi_image_free(image_buffer);
        return 0;
    }
    filesystem::path file_path;
    SoftwareFrameWithMemory image;
    int width;
    int height;
    int channels;
};


int main(int argc, char *argv[])
{
    cxxopts::Options options("png_merge", "a png tool to package multi png images to single");
    options.add_options()
            ("o,output","output file name. xxx.png for merged png, xxx.json for coordinate manifest,if not specify,will use directory name",cxxopts::value<std::string>()->default_value(""))
            ("d,directory", "input directory,all png file in the dir will be merged",cxxopts::value<std::string>()->default_value(""))
            ("p,pot", "POT mode, if true, output size will be power-of-two(256,512,1024...))",cxxopts::value<bool>()->default_value("false"))
            ("s,size", "max output size limited, by size*size",cxxopts::value<int32_t>()->default_value("2048"));;
    auto result = options.parse(argc,argv);

    std::string filename = result["output"].as<std::string>();
    std::string dir = result["directory"].as<std::string>();
    int32_t max_size = result["size"].as<int32_t>();
    bool pot = result["pot"].as<bool>();

    if(dir.empty()){
        fprintf(stderr,"%s\n directory must be specify by -d or --directory\n",options.help().c_str());
        return 0;
    }

    filesystem::path image_dir(dir);

    if(filename.empty()){
        filename = image_dir.filename().string();
    }

    mr::tio::SoftwareFramePacker packer;
    packer.create(0,kAspectCrop,max_size);
    for (auto const& dir_entry : std::filesystem::directory_iterator(image_dir))
    {
        if(!dir_entry.is_regular_file())
            continue;
        filesystem::path file(dir_entry);
//        if(file.extension() != ".png" && file.extension() != ".PNG")
//            continue;
        Image image(file);
        if(image.load() < 0){
            continue;
        }
        packer.add_origin(image.image,image.file_path.filename().string());
    }
    packer.finish(filename);
    return 0;
}

