#include <fstream>
#include <glad/egl.h>
#include <glad/gl.h>
#include <cstring>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <nlohmann/json.hpp>
#include <glad/gl.h>
#include <stb/stb_image.h>
#include "example_imgui.h"
//#include <../../../third_party/sources/mr-common/imgui_mr.cpp>

MR_MR_SDL_RUNNER_SHOWCASE(ImGuiExample)


ImGuiExample::ImGuiExample()
{
    name_ = "Imgui Example";
    describe_ = "mountain ripper Imgui widgets showcase";
}

int32_t ImGuiExample::on_set_params(cxxopts::Options &options)
{
    return 0;
}

int32_t ImGuiExample::on_pre_init(cxxopts::ParseResult &options_result, uint32_t &window_flags)
{
    //window_flags |= SDL_WINDOW_MAXIMIZED;
    return 0;
}

int32_t ImGuiExample::on_init(void *window,int width, int height)
{
    width_ = width;
    height_ = height;
    resized_ = true;

    timeline_.insert(timeline_.size(),{20000,"Alone.mp4"});
    timeline_.insert(timeline_.size(),{10000,"MrChu.mp4"});
    timeline_.insert(timeline_.size(),{30000,"NoNoNo.mp4"});
    timeline_.resize(20);

    GraphicTexture texture;
    texture.api = mr::tio::kGraphicApiOpenGL;
    std::map<std::string,CropArea> areas;
    auto ret = TextureIO::create_texture("images.json",texture,areas);
    for(auto& item : areas){
        mr::TextureHolder::Image image;
        auto& area = item.second;
        image.pos= ImVec2(area.crop_x,area.crop_y);
        image.size= ImVec2(area.crop_width,area.crop_height);
        image.size_totle = ImVec2(texture.width,texture.height);
        image.texture = (ImTextureID)texture.context[0];
        mr::TextureHolder::add_image(item.first,image);
    }


    return 0;
}

int32_t ImGuiExample::on_deinit()
{
    return 0;
}


int32_t ImGuiExample::on_frame()
{
    //convert_to_target_frames();

    glClearColor(.5, .75, .5, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    static float values1[2] = {100.0,200.0};
    static float values2[2] = {400.0,700.0};
    static float values3[2] = {200.0,500.0};
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);
    ImGui::SetNextWindowBgAlpha(0.35);
    ImGui::Begin("TimeLine");

    auto window = ImGui::GetCurrentWindow();

    timeline_.show(64);

    auto& images = mr::TextureHolder::all_images();
    for(const auto& image : images){
        ImGui::Text("%s:",image.first.c_str());
        ImGui::Image(image.second.texture,
                     image.second.size,
                     image.second.top_left(),
                     image.second.bottom_right());
    }

    ImGui::End();
    ImGui::PopStyleVar(2);



    return 0;
}

void ImGuiExample::button_callback(int bt, int type, int clicks, double x, double y)
{

}


void ImGuiExample::cursor_callback(double x, double y)
{
}

void ImGuiExample::key_callback(int key, int type, int scancode, int mods)
{

}

void ImGuiExample::error_callback(int err, const char *desc)
{
}

void ImGuiExample::resize_callback(int width, int height)
{
    width_ = width;
    height_ = height;
    resized_ = true;
}


void ImGuiExample::command(std::string command)
{

}

