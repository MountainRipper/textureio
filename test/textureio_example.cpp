#include "textureio_example.h"
#include <glad/egl.h>
#include <glad/gl.h>
#include <cstring>
#include <imgui.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "logger.h"
#include "test_pic.h"
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

static inline unsigned int decode_85byte(char c){ return c >= '\\' ? c-36 : c-35; }
static void  decode_base85(const unsigned char* src, unsigned char* dst)
{
    while (*src)
    {
        unsigned int tmp = decode_85byte(src[0]) + 85 * (decode_85byte(src[1]) + 85 * (decode_85byte(src[2]) + 85 * (decode_85byte(src[3]) + 85 * decode_85byte(src[4]))));
        dst[0] = ((tmp >> 0) & 0xFF); dst[1] = ((tmp >> 8) & 0xFF); dst[2] = ((tmp >> 16) & 0xFF); dst[3] = ((tmp >> 24) & 0xFF);   // We can't assume little-endianness.
        src += 5;
        dst += 4;
    }
}
TextureioExample::TextureioExample()
{

}
int32_t TextureioExample::on_init(void *window)
{
    int channels = 3;//as rgb
    auto image = stbi_load_from_memory((uint8_t*)test_pic_data,test_pic_size,&pic_width_,&pic_height_,&channels,channels);
    assert(channels == 3);
    test_picture_ = SoftwareFrameWithMemory{{kSoftwareFormatRGB24,pic_width_,pic_height_}};
    test_picture_.alloc();
    memcpy(test_picture_.data_buffer_,image,pic_width_*pic_height_*3);
    stbi_image_free(image);

    textures_[0] = create_texture(128,128,0);
    textures_[1] = create_texture(128,128,128);
    textures_[2] = create_texture(128,128,128);

    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"create_texture");

    std::string vertex_shader = g_vs_video;
    auto version = (const char*)glGetString(GL_VERSION);
    if(version && strstr(version,"OpenGL ES"))
        vertex_shader = std::string("#version 300 es") + vertex_shader;
    else
        vertex_shader = std::string("#version 430") + vertex_shader;


    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"glGetString");

    for(int index = 0; index < kSoftwareFormatCount; index++){
        auto fragment_shader = TextureIO::reference_shader_software(kGraphicApiOpenGL,static_cast<SoftwareFrameFormat>(index));
        MP_INFO("{}",fragment_shader.c_str());
        programs_[index] = create_program(vertex_shader.c_str(),fragment_shader.c_str());
    }
    return 0;
}

int32_t TextureioExample::on_deinit()
{
    return 0;
}

int32_t TextureioExample::on_frame()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    int index=0;
    int row_columns = 5;
    float item_percent = 1.0 / row_columns;
    for(int row = 0; row < row_columns; row++){
        for(int column = 0; column < row_columns; column++){
            if(index >= kSoftwareFormatCount)
                break;

            SoftwareFrameFormat format = static_cast<SoftwareFrameFormat>(index);

            GraphicTexture texture;
            texture.api = kGraphicApiOpenGL;
            for(int index = 0; index < 3; index++){
                texture.context[index] = textures_[index];
                texture.flags[index] = texture_unit_base_+index;
            }

            SoftwareFrameWithMemory dest{{format,test_picture_.width,test_picture_.height}};
            dest.alloc();

            software_converter_.convert(test_picture_,dest);
            glUseProgram(programs_[index]);
            //TextureIO::software_frame_to_graphic(dest,texture);


            ImGui::SetNextWindowPos(ImVec2(item_percent*column*width_,item_percent*row*height_));
            ImGui::SetNextWindowBgAlpha(0.15f);
            std::string win_name = "hint-";
            win_name += g_soft_format_names[index];
            ImGui::Begin(win_name.c_str(),NULL,window_flags);
            ImGui::Text("%s", g_soft_format_names[index]);
            ImGui::End();

            index++;
        }
    }
    return 0;
}

void TextureioExample::button_callback(int bt, int action, int mods)
{
}

void TextureioExample::cursor_callback(double x, double y)
{
}

void TextureioExample::key_callback(int key, int scancode, int action, int mods)
{
}

void TextureioExample::char_callback(unsigned int key)
{
}

void TextureioExample::error_callback(int err, const char *desc)
{
}

void TextureioExample::resize_callback(int width, int height)
{
    width_ = width;
    height_ = height;
}

void TextureioExample::scroll_callback(double xoffset, double yoffset)
{
}

void TextureioExample::command(std::string command)
{
}
