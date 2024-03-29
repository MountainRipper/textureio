#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stb/stb_rect_pack.h>
#include <stb/stb_image_write.h>
#include <nlohmann/json.hpp>
#include <libyuv.h>
#include <tio/tio_software_frame.h>

namespace mr::tio{


struct SoftwareFramePackerPrivate{
    std::vector<SoftwareFrameWithMemory> frames_;
    std::vector<std::string> names_;
    int32_t     item_scale_limit_   = kImagePackerDefaultScaleLimit;
    int32_t     target_size_limit_  = kImagePackerDefaultOutputLimit;
    int8_t      spacing_            = 1;
    int16_t     increase_step_      = 50;
    bool        pow_of_two_         = false;
};

int32_t SoftwareFramePacker::create(int32_t target_size_limit, int32_t item_size_limit, uint8_t spacing, uint16_t increase_step, bool pow_of_two){
    context_ = std::shared_ptr<SoftwareFramePackerPrivate>(new SoftwareFramePackerPrivate());
    context_->item_scale_limit_   = item_size_limit >> 1 << 1;
    context_->target_size_limit_  = target_size_limit;
    context_->spacing_            = spacing;
    context_->increase_step_      = increase_step;
    context_->pow_of_two_         = pow_of_two;

    if(context_->item_scale_limit_ < kImagePackerDefaultScaleLimit)
        context_->item_scale_limit_ = kImagePackerDefaultScaleLimit;
    return 0;
}
int32_t SoftwareFramePacker::add(const SoftwareFrame& frame, const std::string &name, bool keep_aspect_ratio){
    int32_t width  = context_->item_scale_limit_;
    int32_t height = context_->item_scale_limit_;
    if(keep_aspect_ratio){
        if(frame.width >= height){
            float scale = context_->item_scale_limit_ * 1.0 / frame.width;
            width = context_->item_scale_limit_;
            height = int32_t(frame.height * scale);
        }
        else{
            float scale = context_->item_scale_limit_ * 1.0 / frame.height;
            height = context_->item_scale_limit_;
            width = int32_t(frame.width * scale) >> 1 << 1;
        }
    }
    if(width > context_->target_size_limit_ || height > context_->target_size_limit_)
        return kErrorInvalidFrameSize;
    SoftwareFrameWithMemory small_frame(kSoftwareFormatRGBA32,width,height);
    small_frame.alloc();
    SoftwareFrameConvert::convert(frame,small_frame);
    context_->frames_.push_back(small_frame);
    context_->names_.push_back(name);
    return 0;
}

int32_t SoftwareFramePacker::add_origin(const SoftwareFrame &frame, const std::string &name)
{
    if(frame.width > context_->target_size_limit_ || frame.height > context_->target_size_limit_)
        return kErrorInvalidFrameSize;
    SoftwareFrameWithMemory small_frame(kSoftwareFormatRGBA32,frame.width,frame.height);
    small_frame.alloc();
    SoftwareFrameConvert::convert(frame,small_frame);
    context_->frames_.push_back(small_frame);
    context_->names_.push_back(name);
    return 0;
}

int32_t SoftwareFramePacker::finish(const std::string& filename){
    nlohmann::json json_menifest;
    std::filesystem::path output_path(filename);
    std::vector<stbrp_rect> rects;
    json_menifest["images"] = nlohmann::json::array();

    uint32_t totle_pixels = 0;
    int32_t width_max = 0;
    int32_t index = 0;
    for (auto const& frame : context_->frames_)
    {
        stbrp_rect rect;
        rect.id = index;
        rect.w = frame.width + context_->spacing_;
        rect.h = frame.height + context_->spacing_;
        rect.x = 0;
        rect.y = 0;
        rect.was_packed = 0;
        rects.push_back(rect);

        if(rect.w > width_max)
            width_max = rect.w;
        if(rect.h > width_max)
            width_max = rect.h;

        totle_pixels += rect.w * rect.h;

        index++;
    }

    bool pot = context_->pow_of_two_;

    auto calc_target_size = [](int32_t pixels,int32_t max_size,int32_t min_size = 0,bool pot = false,int32_t step = 100) ->int32_t {
        int32_t target_size =  pot ? 128 : 100;
        while ((target_size * target_size) < (pixels*1.1) || ( min_size > 0 && target_size < min_size)) {
            int32_t next_size = pot ? target_size * 2 : target_size + step;
            if(next_size > max_size)
                break;
            else
                target_size = next_size;
        }
        return target_size;
    };
    //decide a target size
    int target_size = calc_target_size(totle_pixels,context_->target_size_limit_,width_max,context_->pow_of_two_,context_->increase_step_);

    int nodes_count = 4096;
    struct stbrp_node nodes[4096];

    int32_t image_index = 0;
    std::vector<stbrp_rect> rects_unpacked = rects;
    std::vector<stbrp_rect> rects_packed;
    do{
        stbrp_context context;
        stbrp_init_target(&context, target_size, target_size, nodes, nodes_count);
        stbrp_setup_allow_out_of_mem (&context, 1);
        stbrp_setup_heuristic(&context, 1);
        stbrp_pack_rects(&context, rects.data(), rects.size());

        bool try_next_size_again = false;
        for (const auto& rect : rects)
        {
            if(rect.was_packed == 0){
                int32_t next_size = pot ? target_size*2 : target_size + context_->increase_step_;
                if(next_size <= context_->target_size_limit_){
                    target_size = next_size;
                    try_next_size_again = true;
                    fprintf(stderr,"item not packaged, increase target size to:%d\n",target_size);
                    continue;
                }
                else
                    break;
            }
        }
        if(try_next_size_again)
            continue;

        rects_unpacked.clear();
        rects_packed.clear();
        width_max = 0;
        for (const auto& rect : rects)
        {
            fprintf(stderr,"rect %i (%hu*%hu) (%hu,%hu) was_packed=%i\n", rect.id, rect.w, rect.h, rect.x, rect.y, rect.was_packed);
            if(rect.was_packed == 0){
                rects_unpacked.push_back(rect);
                if(rect.w > width_max)
                    width_max = rect.w;
                if(rect.h > width_max)
                    width_max = rect.h;
            }
            else
                rects_packed.push_back(rect);
        }

        rects = rects_unpacked;
        if(rects_packed.size()){
            SoftwareFrameWithMemory output_image(kSoftwareFormatRGBA32,target_size,target_size);
            output_image.alloc();
            output_image.clear();

            nlohmann::json json_image;
            json_image["imageAreas"] = {};
            for (const auto& rect : rects_packed)
            {
                auto& image = context_->frames_[rect.id];
                libyuv::ARGBCopy(image.data[0],image.linesize[0],
                                 output_image.data[0] + (output_image.linesize[0] * rect.y + rect.x * 4),output_image.linesize[0],
                                 image.width,image.height);

                totle_pixels -= image.width * image.height;
                json_image["imageAreas"][context_->names_[rect.id]] = {{"x",rect.x},{"y",rect.y},{"w",image.width},{"h",image.height}};
            }

            auto image_path = output_path;
            if(image_index > 0){
                std::stringstream ss;
                ss << image_path.stem().u8string() << "-" << image_index;
                image_path.replace_filename(ss.str());
            }
            image_path.replace_extension(".png");
            stbi_write_png(image_path.u8string().c_str(),target_size,target_size,4,output_image.data_buffer_,output_image.linesize[0]);
            fprintf(stderr,"target size:%d\n",target_size);

            json_image["imageFile"]  = image_path.filename();
            json_image["imageWidth"]  = target_size;
            json_image["imageHeight"]  = target_size;

            json_menifest["images"].push_back(json_image);

            image_index++;
            //reset target size
            target_size = target_size = calc_target_size(totle_pixels,context_->target_size_limit_,width_max,context_->pow_of_two_);
        }
    }while(rects_unpacked.size());

    std::ofstream ofs(output_path.replace_extension(".json").u8string());
    ofs << std::setw(2) << json_menifest << std::endl;
    return 0;
}


}
