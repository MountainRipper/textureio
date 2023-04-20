#include "example_textureio.h"
#include <glad/egl.h>
#include <glad/gl.h>
#include <cstring>
#include <stb/stb_image.h>
#include "test_pic.h"
#include "test_png.h"
#include "glad/gl.h"
#include <imgui.h>
#include <imgui_internal.h>

MR_MR_SDL_RUNNER_SHOWCASE(TextureioExample)

static const std::string g_vs_video = R"(
  precision mediump float;
  out vec2 v_texCoord;
  void main() {
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    v_texCoord.x = (x+1.0)*0.5;
    v_texCoord.y = (y+1.0)*0.5;
    gl_Position = vec4(x, -y, 0, 1);
})";

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

    convert_to_target_frames();

    textures_[0] = create_texture(128,128,0);
    textures_[1] = create_texture(128,128,128);
    textures_[2] = create_texture(128,128,128);

    MR_GL_CHECK(mr::Logger::kLogLevelError,"create_texture");

    create_programs();

    glGenVertexArrays(1, &g_vao);
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

    GraphicTexture texture;
    texture.api = kGraphicApiOpenGL;
    for(int index = 0; index < 3; index++){
        texture.context[index] = textures_[index];
        texture.flags[index] = texture_unit_base_ + index;
    }

    auto fill_texture_param = [this](SoftwareFrameFormat format,GraphicTexture& param){

    };
    if(show_source_){
        texture.program = programs_[source_format_];
        glUseProgram(texture.program);
        glFinish();

        MR_TIMER_NEW(timer);
        TextureIO::software_frame_to_graphic(source_image_,texture,(SamplerMode)sampler_mode_);
        glFinish();
        float upload_ms = MR_TIMER_MS_RESET(timer);

        glBindVertexArray(g_vao);
        glViewport(0,0,width_,height_);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glFinish();
        float render_ms = MR_TIMER_MS(timer);

        ImGui::SetNextWindowPos(ImVec2(10,10));
        ImGui::SetNextWindowBgAlpha(0.15f);
        std::string win_name = "hint-";
        win_name += g_soft_format_names[index];
        ImGui::Begin(win_name.c_str(),NULL,window_flags);
        ImGui::Text("%s C:%.2fms U:%.2fms R:%.2fms", g_soft_format_names[source_format_],convert_ms_[index],upload_ms,render_ms);
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
                texture.program = programs_[index];

                glUseProgram(texture.program);
                glFinish();

                MR_TIMER_NEW(timer);
                {
                    TextureIO::software_frame_to_graphic(*frame,texture,(SamplerMode)sampler_mode_);
                    glFinish();
                }
                float upload_ms = MR_TIMER_MS_RESET(timer);

                {
                    glBindVertexArray(g_vao);
                    FrameArea area;
                    area.aspect_crop(item_width-2,item_height-2,frame->width*1.0/frame->height);

                    glViewport(x + area.x,
                               vy + area.y,
                               area.width,
                               area.height);

                    glDrawArrays(GL_TRIANGLES, 0, 3);
                    glFinish();
                }
                float render_ms = MR_TIMER_MS(timer);

                ImGui::SetNextWindowPos(ImVec2(x,y));
                ImGui::SetNextWindowBgAlpha(0.15f);
                std::string win_name = "hint-";
                win_name += g_soft_format_names[index];
                ImGui::Begin(win_name.c_str(),NULL,window_flags);
                ImGui::Text("%s C:%.2fms U:%.2fms R:%.2fms", g_soft_format_names[index],convert_ms_[index]/frames_convert_count_,upload_ms,render_ms);
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
        ImGui::SetNextItemWidth(150);
        ImGui::Combo("InputFormat", &source_format_, g_soft_format_names, IM_ARRAYSIZE(g_soft_format_names));ImGui::SameLine();
        ImGui::Checkbox("  ShowSourceOnly",&show_source_);

        ImGui::Text("Image:"); ImGui::SameLine();
        ImGui::RadioButton("Spectacle", &origin_image_use_, 0); ImGui::SameLine();
        ImGui::RadioButton("Tiger", &origin_image_use_, 1);

        int rotate_old_ = rotate_;
        ImGui::Text("Software Rotate:"); ImGui::SameLine();
        ImGui::RadioButton("0", &rotate_, 0); ImGui::SameLine();
        ImGui::RadioButton("90", &rotate_, 90); ImGui::SameLine();
        ImGui::RadioButton("180", &rotate_, 180); ImGui::SameLine();
        ImGui::RadioButton("270", &rotate_, 270);

        float old_ratio = crop_aspect_ratio_;
        bool crop_mode_old_ = crop_mode_;
        ImGui::Text("Software Aspect Ratio:"); ImGui::SameLine();ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("##ratio",&crop_aspect_ratio_, (crop_aspect_ratio_<=1?0.05:0.2), 0.2, 5.0,"%.2f"); ImGui::SameLine();
        ImGui::Checkbox("Crop",&crop_mode_); ImGui::SameLine();
        if(ImGui::Button("Reset")){
            crop_aspect_ratio_ = 0;
            crop_mode_ = false;
        }

        ImGui::Text("Render Sampler:"); ImGui::SameLine();
        ImGui::RadioButton("Auto", &sampler_mode_, 0); ImGui::SameLine();
        ImGui::RadioButton("Linear", &sampler_mode_, 1); ImGui::SameLine();
        ImGui::RadioButton("Nearest", &sampler_mode_, 2);

        int colorsapce_old_ = colorspace_;
        ImGui::Text("Render ColorSpace:"); ImGui::SameLine();
        ImGui::RadioButton("601", &colorspace_, 1); ImGui::SameLine();
        ImGui::RadioButton("601F", &colorspace_, 2); ImGui::SameLine();
        ImGui::RadioButton("709", &colorspace_, 3); ImGui::SameLine();
        ImGui::RadioButton("709F", &colorspace_, 4); ImGui::SameLine();
        ImGui::RadioButton("2020", &colorspace_, 5); ImGui::SameLine();
        ImGui::RadioButton("2020F", &colorspace_, 6);

        ImGui::End();

        if(origin_image_old != origin_image_use_ || rotate_old_ != rotate_)
            crop_aspect_ratio_ = 0;

        if(format_old != source_format_ || origin_image_old != origin_image_use_ || rotate_old_ != rotate_ || old_ratio != crop_aspect_ratio_ || crop_mode_old_ != crop_mode_){
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

    FrameArea final_size;
    final_size.aspect_crop(convert_width,convert_height,crop_aspect_ratio_);
    final_size.width = (final_size.width+1) / 2 * 2;
    final_size.height = (final_size.height+1) / 2 * 2;

    fprintf(stderr,"Final size %dx%d\n",final_size.width,final_size.height);

    MR_TIMER_NEW(aa);
    for(int index = 0; index < kSoftwareFormatCount; index++){
        SoftwareFrameFormat format = static_cast<SoftwareFrameFormat>(index);
        std::shared_ptr<SoftwareFrameWithMemory> dest = std::shared_ptr<SoftwareFrameWithMemory>(new SoftwareFrameWithMemory(format,final_size.width,final_size.height));
        dest->alloc();
        target_frames_[index] = dest;

        MR_TIMER_NEW(convert_timer);
        software_converter_.convert(source_image_,*dest,(RotationMode)rotate_,crop_mode_);
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
    std::string vertex_shader = g_vs_video;
    auto version = (const char*)glGetString(GL_VERSION);
    if(version && strstr(version,"OpenGL ES"))
        vertex_shader = std::string("#version 300 es") + vertex_shader;
    else
        vertex_shader = std::string("#version 330") + vertex_shader;


    MR_GL_CHECK(mr::Logger::kLogLevelError,"glGetString");

    for(int index = 0; index < kSoftwareFormatCount; index++){

        if(programs_[index] != 0){
            glDeleteProgram(programs_[index]);
        }
        auto fragment_shader = TextureIO::reference_shader_software(kGraphicApiOpenGL,static_cast<SoftwareFrameFormat>(index),(YuvColorSpace)colorspace_);
        if(version && strstr(version,"OpenGL ES"))
            fragment_shader = std::string("#version 300 es") + fragment_shader;
        else
            fragment_shader = std::string("#version 330") + fragment_shader;
        //MR_INFO("{}",fragment_shader.c_str());
        programs_[index] = create_program(vertex_shader.c_str(),fragment_shader.c_str());
    }
}
