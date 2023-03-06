#include "textureio_example.h"
#include <glad/egl.h>
#include <glad/gl.h>
#include <glad/glx.h>
#include <cstring>
#include "logger.h"

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
int32_t TextureioExample::on_init(void *window)
{
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

    for(int index = 0; index < kSoftwareFrameCount; index++){
        auto fragment_shader = TextureIO::reference_shader_software(kGraphicApiOpenGL,static_cast<SoftwareFrameFormat>(index));
        MP_INFO("{}",fragment_shader.c_str());
        program_[index] = create_program(vertex_shader.c_str(),fragment_shader.c_str());
    }
    return 0;
}

int32_t TextureioExample::on_deinit()
{
    return 0;
}

int32_t TextureioExample::on_frame()
{
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
}

void TextureioExample::scroll_callback(double xoffset, double yoffset)
{
}

void TextureioExample::command(std::string command)
{
}
