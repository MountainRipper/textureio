#include "example_textureio.h"
#include <glad/egl.h>
#include <glad/gl.h>
#include <cstring>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include "test_pic.h"
#include "test_png.h"
#include "glad/gl.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <mrcommon/logger.h>
MR_MR_SDL_RUNNER_SHOWCASE(TextureioExample)


TextureioExample::TextureioExample()
{

}

int32_t TextureioExample::on_set_params(cxxopts::Options &options)
{
    return 0;
}

int32_t TextureioExample::on_pre_init(cxxopts::ParseResult &options_result,uint32_t& window_flags)
{
    return 0;
}

int32_t TextureioExample::on_init(void *window,int width, int height)
{
    width_ = width;
    height_ = height;
    resized_ = true;

    int pic_width = 0;
    int pic_height = 0;
    int channels = 3;//as rgb
    auto image = stbi_load_from_memory((uint8_t*)test_pic_data,test_pic_size,&pic_width,&pic_height,&channels,channels);
    assert(channels == 3);
    auto copy_temp = SoftwareFrameWithMemory(kSoftwareFormatRGB24,(uint32_t)pic_width,(uint32_t)pic_height);
    copy_temp.attach(image);
    origin_image_[0] = copy_temp.clone_new();
    stbi_image_free(image);

    channels = 4;
    image = stbi_load_from_memory((uint8_t*)test_png_data,test_png_size,&pic_width,&pic_height,&channels,channels);
    assert(channels == 4);
    copy_temp = SoftwareFrameWithMemory(kSoftwareFormatRGBA32,(uint32_t)pic_width,(uint32_t)pic_height);
    copy_temp.attach(image);
    origin_image_[1] = copy_temp.clone_new();
    stbi_image_free(image);

    for(int index = 0; index < kSoftwareFormatCount; index++){
        format_names[index] = g_software_format_info[index].name;
    }
    convert_to_target_frames();

    textures_[0] = create_texture(128,128,0);
    textures_[1] = create_texture(128,128,128);
    textures_[2] = create_texture(128,128,128);

    MR_GL_CHECK(mr::Logger::kLogLevelError,"create_texture");

    create_programs();

    glBindVertexArray(0);
    return 0;
}

int32_t TextureioExample::on_deinit()
{
    return 0;
}

int32_t TextureioExample::on_frame()
{
    //convert_to_target_frames();

    glClearColor(.5, .75, .5, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    int index=0;
    int row_columns = 5;
    float item_percent = 1.0 / row_columns;
    int item_width = width_*item_percent;
    int item_height = height_*item_percent;

    auto fill_texture_param = [this](SoftwareFrameFormat format,GraphicTexture& param){

    };
    static float rotate = 0;
    rotate += 1;
    if(rotate > 360)
        rotate = 0;

    if(show_source_){

        float upload_ms = 0;
        auto shader = programs_[source_format_];
        //glFinish();
        MR_TIMER_NEW(timer);
        if(shader != nullptr){
            shader->use();
            //glFinish();
            upload_ms = MR_TIMER_MS_RESET(timer);

            mr::tio::ReferenceShader::RenderParam param{0,0,int32_t(width_),int32_t(height_),render_rotate_,render_scale_x_,render_scale_y_,render_offset_x_,render_offset_y_};
            if(render_fill_mode_ != -1){
                param.fill_with((FillMode)render_fill_mode_,width_,height_,source_image_.width,source_image_.height);
            }

            shader->render(source_image_,param,(SamplerMode)sampler_mode_);
            //glFinish();
        }

        float render_ms = MR_TIMER_MS(timer);

        ImGui::SetNextWindowPos(ImVec2(10,10));
        ImGui::SetNextWindowBgAlpha(0.15f);
        std::string win_name = "hint-";
        win_name += g_software_format_info[index].name;
        ImGui::Begin(win_name.c_str(),NULL,window_flags);
        ImGui::Text("%s C:%.2fms U:%.2fms R:%.2fms", g_software_format_info[source_format_].name,convert_ms_[index],upload_ms,render_ms);
        ImGui::End();
    }
    else{
        for(int row = 0; row < row_columns; row++){
            for(int column = 0; column < row_columns; column++){
                int x = item_percent*column*width_;
                int y = item_percent*row*height_;
                int vy = height_ - y - item_height;

                if(index >= kSoftwareFormatCount)
                    break;

                SoftwareFrameFormat format = static_cast<SoftwareFrameFormat>(index);
                std::shared_ptr<SoftwareFrameWithMemory> frame = target_frames_[index];

                auto shader = programs_[index];
                if(!shader)
                    continue;

                shader->use();

                //glFinish();
                MR_TIMER_NEW(timer);

                {
                    FrameArea area;
                    area.aspect_crop(item_width-2,item_height-2,frame->width*1.0/frame->height);

                    int32_t view_x = x + area.x;
                    int32_t view_y = vy + area.y;
                    int32_t view_w = area.width;
                    int32_t view_h = area.height;
                    mr::tio::ReferenceShader::RenderParam param{view_x,view_y,view_w,view_h,render_rotate_,render_scale_x_,render_scale_y_,render_offset_x_,render_offset_y_};

                    if(render_fill_mode_ != -1){
                        param.fill_with((FillMode)render_fill_mode_,item_width-2,item_height-2,frame->width,frame->height);
                        param.view_x += x;
                        param.view_y += vy;
                    }

                    shader->render(*frame,param,(SamplerMode)sampler_mode_);
                    //glFinish();
                }
                float render_ms = MR_TIMER_MS(timer);

                ImGui::SetNextWindowPos(ImVec2(x,y));
                ImGui::SetNextWindowBgAlpha(0.15f);
                std::string win_name = "hint-";
                win_name += g_software_format_info[index].name;
                ImGui::Begin(win_name.c_str(),NULL,window_flags);
                ImGui::Text("%s C:%.2fms R:%.2fms", g_software_format_info[index].name,convert_ms_[index]/frames_convert_count_,render_ms);
                ImGui::End();

                index++;
            }
        }
    }



    {
//        window_flags |= ImGuiWindowFlags_NoMove;
        if(resized_){
            resized_ = false;
            auto pos = ImGui::GetMainViewport()->GetCenter();
            pos.x *= 1.4;
            pos.y *= 1.8;
            ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        }

        ImGui::SetNextWindowBgAlpha(0.35f);
        ImGui::Begin("Control Panel",NULL,window_flags);
        int format_old = source_format_;
        int origin_image_old = origin_image_use_;
        ImGui::Text("Image:"); ImGui::SameLine();
        ImGui::RadioButton("Spectacle", &origin_image_use_, 0); ImGui::SameLine();
        ImGui::RadioButton("Tiger", &origin_image_use_, 1);ImGui::SameLine();ImGui::SetNextItemWidth(150);
        ImGui::Combo("Format", &source_format_, format_names, IM_ARRAYSIZE(format_names));ImGui::SameLine();
        ImGui::Checkbox("  ShowSourceOnly",&show_source_);


        int rotate_old_ = rotate_;
        ImGui::Text("Convert Rotate:"); ImGui::SameLine();
        ImGui::RadioButton("0", &rotate_, 0); ImGui::SameLine();
        ImGui::RadioButton("90", &rotate_, 90); ImGui::SameLine();
        ImGui::RadioButton("180", &rotate_, 180); ImGui::SameLine();
        ImGui::RadioButton("270", &rotate_, 270);

        float old_ratio = crop_aspect_ratio_;
        int fill_mode_old_ = convert_fill_mode_;
        ImGui::Text("Convert Aspect Ratio:"); ImGui::SameLine();ImGui::SetNextItemWidth(180);
        char format[64];
        sprintf(format,"%s %dx%d","%.2f",final_size_.width,final_size_.height);
        ImGui::DragFloat("##ratio",&crop_aspect_ratio_, (crop_aspect_ratio_<=1?0.05:0.2), 0.2, 5.0,format); ImGui::SameLine();
        if(ImGui::Button("Reset")){
            crop_aspect_ratio_ = 0;
        }
        ImGui::SameLine();
        ImGui::Text("Fill Mode:"); ImGui::SameLine();
        ImGui::RadioButton("Fill", &convert_fill_mode_, kStretchFill); ImGui::SameLine();
        ImGui::RadioButton("Fit", &convert_fill_mode_, kAspectFit); ImGui::SameLine();
        ImGui::RadioButton("Crop", &convert_fill_mode_, kAspectCrop);

        ImGui::Text("Render Sampler:"); ImGui::SameLine();
        ImGui::RadioButton("Auto", &sampler_mode_, 0); ImGui::SameLine();
        ImGui::RadioButton("Nearest", &sampler_mode_, 1); ImGui::SameLine();
        ImGui::RadioButton("Linear", &sampler_mode_, 2);

        int colorsapce_old_ = colorspace_;
        ImGui::Text("Render ColorSpace:"); ImGui::SameLine();
        ImGui::RadioButton("601", &colorspace_, 1); ImGui::SameLine();
        ImGui::RadioButton("601F", &colorspace_, 2); ImGui::SameLine();
        ImGui::RadioButton("709", &colorspace_, 3); ImGui::SameLine();
        ImGui::RadioButton("709F", &colorspace_, 4); ImGui::SameLine();
        ImGui::RadioButton("2020", &colorspace_, 5); ImGui::SameLine();
        ImGui::RadioButton("2020F", &colorspace_, 6);

        ImGui::Text("Render:"); ImGui::SameLine();ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("scaleX",&render_scale_x_, (abs(render_scale_x_)<=1?0.05:0.2), -5.0, 5.0,"%.2f"); ImGui::SameLine();ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("scaleY",&render_scale_y_, (abs(render_scale_x_)<=1?0.05:0.2), -5.0, 5.0,"%.2f"); ImGui::SameLine();ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("offsetX",&render_offset_x_, 0.025, -1, 1,"%.2f"); ImGui::SameLine();ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("offsetY",&render_offset_y_, 0.025, -1, 1,"%.2f"); ImGui::SameLine();ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("rotate",&render_rotate_, 1, 0, 360,"%.2f");ImGui::SameLine();
        if(ImGui::Button("Reset##renderTransitions")){
            render_scale_x_ = 1;
            render_scale_y_ = 1;
            render_offset_x_ = 0;
            render_offset_y_ = 0;
            render_rotate_ = 0;
        }
        ImGui::Text("Render Fill Mode:"); ImGui::SameLine();
        ImGui::PushID("render_fill_mode");
        ImGui::RadioButton("Custom", &render_fill_mode_, -1); ImGui::SameLine();
        ImGui::RadioButton("Fill", &render_fill_mode_, kStretchFill); ImGui::SameLine();
        ImGui::RadioButton("Fit", &render_fill_mode_, kAspectFit); ImGui::SameLine();
        ImGui::RadioButton("Crop", &render_fill_mode_, kAspectCrop);
        ImGui::PopID();

        ImGui::End();

        if(origin_image_old != origin_image_use_ || rotate_old_ != rotate_)
            crop_aspect_ratio_ = 0;

        if(format_old != source_format_ || origin_image_old != origin_image_use_ || rotate_old_ != rotate_ || old_ratio != crop_aspect_ratio_ || fill_mode_old_ != convert_fill_mode_){
            frames_convert_count_ = 0;
            memset(convert_ms_,0,sizeof(float)*kSoftwareFormatCount);
            convert_to_target_frames();
        }
        if(colorsapce_old_ != colorspace_){
            create_programs();
        }
    }
    return 0;
}

void TextureioExample::button_callback(int bt, int type, int clicks, double x, double y)
{

}


void TextureioExample::cursor_callback(double x, double y)
{
}

void TextureioExample::key_callback(int key, int type, int scancode, int mods)
{

}

void TextureioExample::char_callback(const char *key, int type)
{

}

void TextureioExample::error_callback(int err, const char *desc)
{
}

void TextureioExample::resize_callback(int width, int height)
{
    width_ = width;
    height_ = height;
    resized_ = true;
}


void TextureioExample::command(std::string command)
{
}

void TextureioExample::convert_to_target_frames()
{
    frames_convert_count_++;

    SoftwareFrameWithMemory& image = origin_image_[origin_image_use_];
    source_image_ = SoftwareFrameWithMemory((SoftwareFrameFormat)source_format_,image.width,image.height);
    source_image_.alloc();
    software_converter_.convert(image,source_image_);

    int convert_width = source_image_.width;
    int convert_height = source_image_.height;
    if(rotate_ == 90 || rotate_ == 270){
        std::swap(convert_width,convert_height);
    }
    if(crop_aspect_ratio_ == 0){
        crop_aspect_ratio_ = convert_width * 1.0 / convert_height;
    }

    final_size_.aspect_crop(convert_width,convert_height,crop_aspect_ratio_);
    final_size_.width = (final_size_.width+1) / 2 * 2;
    final_size_.height = (final_size_.height+1) / 2 * 2;

    fprintf(stderr,"Final size %dx%d format:%s\n",final_size_.width,final_size_.height,g_software_format_info[source_format_].name);

    MR_TIMER_NEW(aa);
    for(int index = 0; index < kSoftwareFormatCount; index++){
        SoftwareFrameFormat format = static_cast<SoftwareFrameFormat>(index);
        std::shared_ptr<SoftwareFrameWithMemory> dest = std::shared_ptr<SoftwareFrameWithMemory>(new SoftwareFrameWithMemory(format,final_size_.width,final_size_.height));
        dest->alloc();
        target_frames_[index] = dest;

        MR_TIMER_NEW(convert_timer);
        software_converter_.convert(source_image_,*dest,(RotationMode)rotate_,(FillMode)convert_fill_mode_);
        convert_ms_[index] += MR_TIMER_MS(convert_timer);
    }
    static float totle = 0;
    totle += MR_TIMER_MS(aa);
    if(frames_convert_count_%100 == 0)
        fprintf(stderr,"%.2f\n",totle/frames_convert_count_);
}

void TextureioExample::create_programs()
{
    glUseProgram(0);

    for(int index = 0; index < kSoftwareFormatCount; index++){

        if(programs_[index] != 0){
            programs_[index] = nullptr;
        }

        programs_[index] = TextureIO::create_reference_shader(mr::tio::kGraphicApiOpenGL,(SoftwareFrameFormat)index,(YuvColorSpace)colorspace_);
    }
}
