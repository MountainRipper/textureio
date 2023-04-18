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
#include <nlohmann/json.hpp>
#include <cxxopts.hpp>

using namespace std;

struct Image{
    Image(filesystem::path file_path){
        this->file_path = file_path;
    }
    int32_t load(){
        auto image_buffer = stbi_load(file_path.c_str(),&width,&height,&channels,4);
        if(channels != 4){
            fprintf(stderr,"image:%s not a 4 channel rgba png file\n",file_path.c_str());
            return -1;
        }
        uint32_t image_bytes = width*height*4;
        buffer_ = std::shared_ptr<uint8_t>(new uint8_t[image_bytes],std::default_delete<uint8_t[]>());
        memcpy(buffer_.get(),image_buffer,image_bytes);
        return 0;
    }
    filesystem::path file_path;
    std::shared_ptr<uint8_t> buffer_;
    int width;
    int height;
    int channels;
};

struct RGBA{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

int main(int argc, char *argv[])
{
    cxxopts::Options options("png_merge", "a png tool to package multi png images to single");
    options.add_options()
            ("o,output","output file name. xxx.png for merged png, xxx.json for coordinate manifest,if not specify,will use directory name",cxxopts::value<std::string>()->default_value(""))
            ("d,directory", "input directory,all png file in the dir will be merged",cxxopts::value<std::string>()->default_value(""))
            ("p,pot", "POT mode, if true, output size will be power-of-two(256,512,1024...))",cxxopts::value<bool>()->default_value("false"));
    auto result = options.parse(argc,argv);

    std::string filename = result["output"].as<std::string>();
    std::string dir = result["directory"].as<std::string>();
    bool pot = result["pot"].as<bool>();

    if(dir.empty()){
        fprintf(stderr,"%s\n directory must be specify by -d or --directory\n",options.help().c_str());
        return 0;
    }

    filesystem::path image_dir(dir);

    if(filename.empty()){
        filename = image_dir.filename().string();
    }
    std::vector<Image> images;
    std::vector<stbrp_rect> rects;

    uint32_t totle_pixels = 0;
    for (auto const& dir_entry : std::filesystem::directory_iterator(image_dir))
    {
        if(!dir_entry.is_regular_file())
            continue;
        filesystem::path file(dir_entry);
        if(file.extension() != ".png" && file.extension() != ".PNG")
            continue;
        Image image(file);
        if(image.load() < 0){
            continue;
        }
       images.push_back(image);

       stbrp_rect rect;
       rect.id = images.size()-1;
       rect.w = image.width;
       rect.h = image.height;
       rect.x = 0;
       rect.y = 0;
       rect.was_packed = 0;
       rects.push_back(rect);

       totle_pixels += image.width * image.height;
    }

    int32_t target_size = pot?128:100;
    while ((target_size *target_size) < (totle_pixels*1.1)) {
        target_size = pot ? target_size*2 : target_size+100;
    }

    int nodes_count = 4096;
    struct stbrp_node nodes[4096];

    bool done = true;
    do{
        done = true;
        stbrp_context context;
        stbrp_init_target(&context, target_size, target_size, nodes, nodes_count);
        stbrp_setup_allow_out_of_mem (&context, 1);
        stbrp_pack_rects(&context, rects.data(), rects.size());

        for (int i=0; i< rects.size(); i++)
        {
            fprintf(stderr,"rect %i (%hu,%hu) was_packed=%i\n", rects[i].id, rects[i].x, rects[i].y, rects[i].was_packed);
            if(rects[i].was_packed == 0){
                fprintf(stderr,"item not packaged, increase target size to:%d\n",target_size);
                done = false;
                target_size = pot ? target_size*2 : target_size+100;
            }
        }
    }while(!done);

    RGBA* target_image = new RGBA[target_size*target_size];
    //transparent the target
    int target_stride = target_size * 4;
    memset(target_image,0,target_size*target_size*4);

    nlohmann::json menifest;
    menifest["imageAreas"] = {};
    for (int i=0; i< rects.size(); i++)
    {
        Image& image = images[i];
        stbrp_rect rect = rects[i];
        int source_stride = image.width * 4;
        uint8_t* source_ptr = image.buffer_.get();
        uint8_t* target_ptr = (uint8_t*)(target_image + target_size*rect.y+rect.x);
        for(int y = 0;y < image.height; y++){
            memcpy(target_ptr,source_ptr,source_stride);
            source_ptr += source_stride;
            target_ptr += target_stride;
        }

        menifest["imageAreas"][image.file_path.filename()] = {{"x",rect.x},{"y",rect.y},{"w",image.width},{"h",image.height}};
    }

    std::string image_file = filename + ".png";
    stbi_write_png(image_file.c_str(),target_size,target_size,4,target_image,target_size*4);
    delete [] target_image;
    fprintf(stderr,"target size:%d\n",target_size);

    menifest["imageFile"]  = image_file;
    menifest["imageWidth"]  = target_size;
    menifest["imageHeight"]  = target_size;
    std::ofstream ofs((filename+".json").c_str());
    ofs << std::setw(2) << menifest << std::endl;
    return 0;
}

