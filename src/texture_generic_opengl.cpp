#include <cstring>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tio/tio_hardware_graphic.h>
#include "texture_generic_opengl.h"
using namespace mr::tio;


#ifdef _WIN64
   #include <glad/gl.h>
#elif _WIN32
   #include <glad/gl.h>
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
        #include <glad/gles2.h>
    #elif TARGET_OS_IPHONE
        #include <glad/gles2.h>
    #else
        #include <glad/gl.h>
    #endif
#elif __ANDROID__
    #include <glad/gles2.h>
#elif __linux__
    #include <glad/gl.h>
#elif __unix__
    #include <glad/gl.h>
#elif __posix

#endif

#ifndef GL_LUMINANCE
#define GL_LUMINANCE 0x1909
#endif
#ifndef GL_LUMINANCE_ALPHA
#define GL_LUMINANCE_ALPHA 0x190A
#endif

/*
2---------3
|\        |
|  \      |
|    \    |
|      \  |
|        \|
0---------1
*/
const GLfloat g_vertices[] = {
    -1, -1, 0,
     1, -1, 0,
    -1,  1, 0,
     1,  1, 0 };

/*
in opengl left-bottom is (0,0),but in image buffer, left-top is (0,0), so need v-flip
0---------1
|        /|
|      /  |
|    /    |
|  /      |
|/        |
2-------- 3
*/
const GLfloat g_texture_coordinates[] = {
    0, 1,
    1, 1,
    0, 0,
    1, 0 };

const char* VERTEX_SHADER = R"(
  precision mediump float;
  attribute vec3 position;
  attribute vec2 textureCoord;
  uniform mat4 project;
  uniform mat4 transition;
  out vec2 v_texCoord;
  void main() {
    gl_Position = transition * vec4(position,1.0);
    v_texCoord = textureCoord;
})";

const char* SHADER_HEADER_DEFINE_PRECISION_AND_TEXCOORD = R"(
#ifdef GL_ES
    #define TEXTURE2D texture2D
    #define LOWP lowp
    #define MEDIUMP mediump
    #define HIGHP highp
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
        varying highp vec2 v_texCoord;
    #else
        precision mediump float;
        varying mediump vec2 v_texCoord;
    #endif
#else
    #if __VERSION__ < 130
        #define TEXTURE2D texture2D
    #else
        #define TEXTURE2D texture
    #endif
    #define LOWP
    #define MEDIUMP
    #define HIGHP
    varying vec2 v_texCoord;
#endif
//__SHADER_HEADER_REPLACER__
)";



const char* SHADER_ONE_TEXTURE_PROLOGUE = R"(
uniform sampler2D tex1;
)";

const char* SHADER_TWO_TEXTURE_PROLOGUE = R"(
uniform sampler2D tex1;
uniform sampler2D tex2;
)";
const char* SHADER_THREE_TEXTURE_PROLOGUE = R"(
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
)";

const char* SHADER_FOUR_TEXTURE_PROLOGUE = R"(
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
)";

//many code take from libsdl,SALUTE!!
const char* SHADER_BT601_CONSTANTS = R"(
const vec3 offset = vec3(-0.0627451017, -0.501960814, -0.501960814);
const mat3 matrix = mat3( 1.1644,  1.1644,   1.1644,
                          0,       -0.3918,  2.0172,
                          1.596,   -0.813,   0);
)";
const char* SHADER_BT601FULL_CONSTANTS = R"(
const vec3 offset = vec3(0, -0.501960814, -0.501960814);
const mat3 matrix = mat3( 1,        1,          1,
                          0,        -0.34414,   1.772,
                          1.40200,  -0.71414,   0);
)";


const char* SHADER_BT709_CONSTANTS = R"(
const vec3 offset = vec3(-0.0627451017, -0.501960814, -0.501960814);
const mat3 matrix = mat3( 1.1644,  1.1644,   1.1644,
                          0,      -0.2132,   2.1124,
                          1.7927, -0.5329,   0);
)";
const char* SHADER_BT709FULL_CONSTANTS = R"(
const vec3 offset = vec3(0, -0.501960814, -0.501960814);
const mat3 matrix = mat3( 1,        1,          1,
                          0,        -0.18732,   1.8556,
                          1.5748,   -0.46812,   0);
)";

const char* SHADER_BT2020_CONSTANTS = R"(
const vec3 offset = vec3(-0.0627451017, -0.501960814, -0.501960814);
const mat3 matrix = mat3( 1.164384,  1.164384,   1.164384,
                          0,         -0.187326,  2.14177,
                          1.67867,   -0.65042,   0);
)";
const char* SHADER_BT2020FULL_CONSTANTS = R"(
const vec3 offset = vec3(0, -0.501960814, -0.501960814);
const mat3 matrix = mat3( 1,        1,          1,
                          0,        -0.164553,  1.8814,
                          1.4746,   -0.571353,  0);
)";

const char* SHADER_BODY_I420_I422_I444 = R"(
void main()
{
    MEDIUMP vec3 yuv;
    LOWP vec3 rgb;

    yuv.x = TEXTURE2D(tex1, v_texCoord).r;
    yuv.y = TEXTURE2D(tex2, v_texCoord).r;
    yuv.z = TEXTURE2D(tex3, v_texCoord).r;

    yuv += offset;
    rgb = matrix * yuv;

    gl_FragColor = vec4(rgb, 1);
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_YV12 = R"(
void main()
{
    MEDIUMP vec3 yuv;
    LOWP vec3 rgb;

    yuv.x = TEXTURE2D(tex1, v_texCoord).r;
    yuv.z = TEXTURE2D(tex2, v_texCoord).r;
    yuv.y = TEXTURE2D(tex3, v_texCoord).r;

    yuv += offset;
    rgb = matrix * yuv;

    gl_FragColor = vec4(rgb, 1);
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_NV12_NV16_NV24 = R"(
void main()
{
    MEDIUMP vec3 yuv;
    LOWP vec3 rgb;

    yuv.x = TEXTURE2D(tex1,   v_texCoord).r;
#ifdef GL_ES
    yuv.yz = TEXTURE2D(tex2, v_texCoord).ra;
#else
    yuv.yz = TEXTURE2D(tex2, v_texCoord).rg;
#endif
    yuv += offset;
    rgb = matrix * yuv;

    gl_FragColor = vec4(rgb, 1);
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_NV21_NV61_NV42 = R"(
void main()
{
    MEDIUMP vec3 yuv;
    LOWP vec3 rgb;

    yuv.x = TEXTURE2D(tex1,   v_texCoord).r;
#ifdef GL_ES
    yuv.yz = TEXTURE2D(tex2, v_texCoord).ar;
#else
    yuv.yz = TEXTURE2D(tex2, v_texCoord).gr;
#endif

    yuv += offset;
    rgb = matrix * yuv;

    gl_FragColor = vec4(rgb, 1);
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_YUYV422 = R"(
uniform vec2 videoSize;
void main()
{
    MEDIUMP vec3 yuv;
    LOWP vec3 rgb;

    MEDIUMP vec4 yuyv = TEXTURE2D(tex1, v_texCoord);
    if (mod(round(v_texCoord.x * videoSize.x),2.0) == 1.0) {
        yuv.x = yuyv.r;
    } else {
        yuv.x = TEXTURE2D(tex1, vec2(v_texCoord.x - 1.0 / videoSize.x,v_texCoord.y)).b;
    }
    yuv.y = yuyv.g;
    yuv.z = yuyv.a;

    yuv += offset;
    rgb = matrix * yuv;

    gl_FragColor = vec4(rgb, 1);
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_YVYU422 = R"(
uniform vec2 videoSize;
void main()
{
    MEDIUMP vec3 yuv;
    LOWP vec3 rgb;

    MEDIUMP vec4 yvyu = TEXTURE2D(tex1, v_texCoord);
    if (mod(round(v_texCoord.x * videoSize.x),2.0) == 1.0) {
        yuv.x = yvyu.r;
    } else {
        yuv.x = TEXTURE2D(tex1, vec2(v_texCoord.x - 1.0 / videoSize.x,v_texCoord.y)).b;
    }
    yuv.y = yvyu.a;
    yuv.z = yvyu.g;

    yuv += offset;
    rgb = matrix * yuv;

    gl_FragColor = vec4(rgb, 1);
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_UYVY422 = R"(
uniform vec2 videoSize;
void main()
{
    MEDIUMP vec3 yuv;
    LOWP vec3 rgb;

    MEDIUMP vec4 uyvy = TEXTURE2D(tex1, v_texCoord);
    if (mod(round(v_texCoord.x * videoSize.x),2.0) == 1.0) {
        yuv.x = uyvy.g;
    } else {
        yuv.x = TEXTURE2D(tex1, vec2(v_texCoord.x - 1.0 / videoSize.x,v_texCoord.y)).a;
    }
    yuv.y = uyvy.r;
    yuv.z = uyvy.b;

    yuv += offset;
    rgb = matrix * yuv;

    gl_FragColor = vec4(rgb, 1);
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_YUV444 = R"(
void main()
{
    MEDIUMP vec3 yuv;
    LOWP vec3 rgb;

    yuv.xyz = TEXTURE2D(tex1, v_texCoord).rgb;

    yuv += offset;
    rgb = matrix * yuv;

    gl_FragColor = vec4(rgb, 1);
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_RGB = R"(
void main()
{
    LOWP vec3 rgb = TEXTURE2D(tex1, v_texCoord).rgb;
    gl_FragColor = vec4(rgb, 1);
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_BGR = R"(
void main()
{
    LOWP vec3 rgb;

    rgb = TEXTURE2D(tex1, v_texCoord).bgr;

    gl_FragColor = vec4(rgb, 1);
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_RGBA = R"(
void main()
{
    gl_FragColor = TEXTURE2D(tex1, v_texCoord).rgba;
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_BGRA_IN = R"(
void main()
{
    gl_FragColor = TEXTURE2D(tex1, v_texCoord).rgba;
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_BGRA = R"(
void main()
{
    gl_FragColor = TEXTURE2D(tex1, v_texCoord).bgra;
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_ARGB = R"(
void main()
{
    gl_FragColor = TEXTURE2D(tex1, v_texCoord).gbar;
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_ABGR = R"(
void main()
{
    gl_FragColor = TEXTURE2D(tex1, v_texCoord).abgr;
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_GRAY = R"(
void main()
{
    float r = TEXTURE2D(tex1, v_texCoord).r;
    gl_FragColor = vec4(r,r,r,1);
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_GRAY_ALPHA = R"(
void main()
{
    vec2 ra = TEXTURE2D(tex1, v_texCoord).rg;
    gl_FragColor = vec4(ra.x , ra.x, ra.x, ra.y);
    //__SHADER_BODY_REPLACER__
}
)";

const char* sharder_textures_define_of_count[]={NULL,
    SHADER_ONE_TEXTURE_PROLOGUE,
    SHADER_TWO_TEXTURE_PROLOGUE,
    SHADER_THREE_TEXTURE_PROLOGUE,
    SHADER_FOUR_TEXTURE_PROLOGUE,
};

const char* sharder_colorspace_define_of_type[]={"",
    SHADER_BT601_CONSTANTS,
    SHADER_BT601FULL_CONSTANTS,
    SHADER_BT709_CONSTANTS,
    SHADER_BT709FULL_CONSTANTS,
    SHADER_BT2020_CONSTANTS,
    SHADER_BT2020FULL_CONSTANTS,
};


bool  TextureGenericOpenGL::gles_ = false;
float TextureGenericOpenGL::version_ = 0;
bool  TextureGenericOpenGL::bgra_support_ = false;
std::vector<std::string> TextureGenericOpenGL::extensions_;

TextureGenericOpenGL::TextureGenericOpenGL()
{
#if defined(IMGL3W_IMPL)
    if (imgl3wInit() != 0)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    }
#endif
}

#define VGFX_GL_CHECK(msg) \
{\
    GLenum gl_error = GL_NO_ERROR;\
    while(GLenum err = glGetError()){\
        fprintf(stderr,"OpenGL Call Failed:%d %s\n",err,msg);\
        gl_error = err;\
    }\
}

class ReferenceShaderOpenGL : public ReferenceShader{


    // ReferenceShader interface
public:
    ReferenceShaderOpenGL(){

    }
    virtual ~ReferenceShaderOpenGL(){

        if(program_ > 0)
            glDeleteProgram(program_);
        if(vao_ > 0)
            glDeleteVertexArrays(1,&vao_);
        if(vbo_vertex_ > 0)
            glDeleteBuffers(1,&vbo_vertex_);
        if(vbo_texcoords_ > 0)
            glDeleteBuffers(1,&vbo_texcoords_);
    }
    virtual std::string shader(ShaderType type) override
    {
        if(type == mr::tio::kShaderTypeVertex){
            return vertex_shader_;
        }
        else if(type == mr::tio::kShaderTypeFragment){
            return fragment_shader_;
        }
        return "";
    }
    virtual uint64_t program() override
    {
        return program_;
    }
    virtual int32_t use() override {
        if(program_ > 0){
            glUseProgram(program_);
            return 0;
        }
        return kErrorInvalidProgram;
    }
    virtual int32_t render(const GraphicTexture &textures, const RenderParam &param) override
    {
        glBindVertexArray(vao_);

        float xScaleImg = 1.0f;
        float yScaleImg = xScaleImg / (textures.width * 1.0 / textures.height);

        float xScaleView = 1.0f;
        float yScaleView = param.view_width * 1.0 / param.view_height;
        glm::mat4 trasition(1.0f);
        trasition = glm::translate(trasition, glm::vec3(param.offset_x, param.offset_y, 0));
        trasition = glm::scale(trasition, glm::vec3(xScaleView, yScaleView, 1.0f));
        trasition = glm::rotate(trasition, glm::radians(param.rotate), glm::vec3(0, 0, 1));
        trasition = glm::scale(trasition, glm::vec3(xScaleImg, yScaleImg, 1.0f));
        trasition = glm::scale(trasition,glm::vec3(param.scale_x, param.scale_y, 1.0));

        glUniformMatrix4fv(uniform_transition_,1,GL_FALSE,glm::value_ptr(trasition));

        for(int index = 0; index < 4; index++){
            if(uniform_textures_[index] >= 0 && textures.flags[index] > 0)
                glUniform1i(uniform_textures_[index], textures.flags[index]);
        }

        if(uniform_video_size_ >= 0){
            glUniform2f(uniform_video_size_,textures.width,textures.height);
        }

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        return 0;
    }

    int32_t create(SoftwareFrameFormat format, YuvColorSpace color_space){
        format_ = format;

        fragment_shader_ = TextureGenericOpenGL::reference_fragment(format,color_space);
        if(TextureGenericOpenGL::gles_){
            vertex_shader_   = std::string("#version 300 es") + VERTEX_SHADER;
            fragment_shader_ = std::string("#version 300 es") + fragment_shader_;
        }
        else{
            vertex_shader_   = std::string("#version 330") + VERTEX_SHADER;
            fragment_shader_ = std::string("#version 330") + fragment_shader_;
        }

        program_ = create_program(vertex_shader_.c_str(),fragment_shader_.c_str());
        if(program_ == 0)
            return kErrorInvalidProgram;

        auto attrib_position = glGetAttribLocation(program_, "position");
        auto attrib_texture_coords = glGetAttribLocation(program_, "textureCoord");
        uniform_transition_ = glGetUniformLocation(program_,"transition");
        uniform_video_size_ = glGetUniformLocation(program_, "videoSize");

        const char* texture_uniform_name[] = {"tex1","tex2","tex3","tex4"};

        const SoftwareFormatPlaner& planers = *TextureIO::planers_of_software_frame(format_);
        for(int index = 0; index < planers.count; index++){
            uniform_textures_[index] = glGetUniformLocation(program_, texture_uniform_name[index]);
        }

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_vertex_);
        glGenBuffers(1, &vbo_texcoords_);

        glBindVertexArray(vao_);

        glEnableVertexAttribArray(attrib_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_);
        glBufferData(GL_ARRAY_BUFFER,sizeof(g_vertices),g_vertices,GL_STATIC_DRAW);
        glVertexAttribPointer(attrib_position, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);

        glEnableVertexAttribArray(attrib_texture_coords);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_);
        glBufferData(GL_ARRAY_BUFFER,sizeof(g_texture_coordinates),g_texture_coordinates,GL_STATIC_DRAW);
        glVertexAttribPointer(attrib_texture_coords, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0);

        glBindVertexArray(0);
        return 0;
    }
private:
    GLuint create_program(const char* vss, const char* fss){

        auto vert = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert, 1, &vss, nullptr);
        glCompileShader(vert);
        print_shader_compile_info(vert);

        auto frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag, 1, &fss, nullptr);
        glCompileShader(frag);
        print_shader_compile_info(frag);

        auto prog = glCreateProgram();
        glAttachShader(prog, vert);
        glAttachShader(prog, frag);
        glLinkProgram(prog);

        glDeleteShader(vert);
        glDeleteShader(frag);
        return prog;
    }
    uint32_t print_shader_compile_info(uint32_t shader) {

      GLint status = 0;
      GLint count = 0;
      GLchar* error = NULL;

      glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
      if(status) {
        return 0;
      }

      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &count);
      if (0 == count) {
        return 0;
      }

      error = (GLchar*) malloc(count);
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &count);
      if(count <= 0) {
        free(error);
        error = NULL;
        return 0;
      }

      glGetShaderInfoLog(shader, count, NULL, error);

      fprintf(stderr,"SHADER COMPILE ERROR\n");
      fprintf(stderr,"--------------------------------------------------------\n");
      fprintf(stderr,"%s\n", error);
      fprintf(stderr,"--------------------------------------------------------\n");

      free(error);
      error = NULL;

      return -1;
    }
private:
    SoftwareFrameFormat format_ = kSoftwareFormatNone;
    std::string fragment_shader_;
    std::string vertex_shader_;
    GLuint program_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_vertex_ = 0;
    GLuint vbo_texcoords_ = 0;
    GLint uniform_transition_ = -1;
    GLint uniform_video_size_ = -1;
    GLint uniform_textures_[4] = {-1,-1,-1,-1};
};

int32_t TextureGenericOpenGL::upload(const SoftwareFrame &frame, GraphicTexture &texture, SamplerMode sampler_mode)
{
    get_capability();
    const SoftwareFormatPlaner& planers = *TextureIO::planers_of_software_frame(frame.format);

    const GLuint channel_foramt_core[5] = {0,GL_RED,GL_RG,GL_RGB,GL_RGBA};
    const GLuint channel_foramt_es[5] = {0,GL_LUMINANCE,GL_LUMINANCE_ALPHA,GL_RGB,GL_RGBA};
    const GLuint* channel_format = gles_ ? channel_foramt_es : channel_foramt_core;


    for(int index = 0; index < planers.count; index++){
        if(index >= planers.count)
            break;
        if(texture.context[index] == 0)
            return kErrorInvalidTextureId;

        const Planer& planer = planers.planes[index];
        uint32_t format = channel_format[planer.channels];
        uint32_t linesize = frame.line_size[index];

        if(frame.format == kSoftwareFormatBGRA32 && bgra_support_){
            format = GL_BGRA;
        }        

        glActiveTexture(GL_TEXTURE0 + texture.flags[index]);
        VGFX_GL_CHECK("TextureGenericOpenGL::upload glActiveTexture")

        glPixelStorei(GL_UNPACK_ALIGNMENT, _tio_max_align(linesize));
        VGFX_GL_CHECK("TextureGenericOpenGL::upload glPixelStorei(GL_UNPACK_ALIGNMENT)")

        glPixelStorei(GL_UNPACK_ROW_LENGTH,linesize/planer.channels);
        VGFX_GL_CHECK("TextureGenericOpenGL::upload glPixelStorei(GL_UNPACK_ROW_LENGTH)")

        glBindTexture(GL_TEXTURE_2D, texture.context[index]);
        VGFX_GL_CHECK("TextureGenericOpenGL::upload glBindTexture")

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLint gl_sampler_filter = GL_LINEAR;
        if(sampler_mode == kSamplerAuto){
            if(frame.format == kSoftwareFormatYUYV422 || frame.format == kSoftwareFormatYVYU422 || frame.format == kSoftwareFormatUYVY422){
                gl_sampler_filter = GL_NEAREST;
            }
        }
        else if(sampler_mode == kSamplerNearest)
            gl_sampler_filter = GL_NEAREST;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_sampler_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_sampler_filter);

        glTexImage2D(GL_TEXTURE_2D,
                   0,
                   format,
                   frame.width*planer.scale_x,
                   frame.height*planer.scale_y,
                   0,
                   format,
                   GL_UNSIGNED_BYTE,
                   frame.data[index]);
        VGFX_GL_CHECK("TextureGenericOpenGL::upload glTexImage2D")        
    }

    texture.width = frame.width;
    texture.height = frame.height;
    return 0;
}

uint64_t TextureGenericOpenGL::create_texture(const SoftwareFrame &frame,GraphicTexture& texture,SamplerMode sampler_mode)
{
    int planes = g_software_format_planers[frame.format].count;
    GLuint texture_ids[4] = {0};
    glGenTextures(planes,texture_ids);

    texture.api = kGraphicApiOpenGL;
    for(int index = 0; index < planes; index++){
        texture.context[index] = texture_ids[index];
        texture.flags[0] = 1 + index;
        if(texture_ids[index] == 0)
            return kErrorAllocTexture;
    }
    upload(frame,texture,kSamplerLinear);

    return 0;
}

int32_t TextureGenericOpenGL::release_texture(uint64_t texture_id)
{
    GLuint id = texture_id;
    if(texture_id){
        glDeleteTextures(1,&id);
        return 0;
    }
    return kErrorInvalidTextureId;
}

std::shared_ptr<ReferenceShader> TextureGenericOpenGL::create_reference_shader(SoftwareFrameFormat format,YuvColorSpace color_space)
{
    auto shader = new ReferenceShaderOpenGL();
    if(shader->create(format,color_space) < 0)
        return std::shared_ptr<ReferenceShader>();
    return std::shared_ptr<ReferenceShader>(shader);
}
std::string TextureGenericOpenGL::reference_fragment(SoftwareFrameFormat format, YuvColorSpace color_space)
{
    get_capability();
    const SoftwareFormatPlaner& planers = *TextureIO::planers_of_software_frame(format);

    std::string shader_string = SHADER_HEADER_DEFINE_PRECISION_AND_TEXCOORD;

    shader_string += sharder_textures_define_of_count[planers.count];

    if(format >= kSoftwareFormatYUVStart && format <= kSoftwareFormatYUVEnd){
        shader_string += sharder_colorspace_define_of_type[color_space];
    }

    switch (format) {
    //420 formats
    case kSoftwareFormatI420:
    shader_string += SHADER_BODY_I420_I422_I444;
        break;
    case kSoftwareFormatYV12:
        shader_string += SHADER_BODY_YV12;
        break;
    case kSoftwareFormatNV12:
    shader_string += SHADER_BODY_NV12_NV16_NV24;
        break;///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    case kSoftwareFormatNV21:
        shader_string += SHADER_BODY_NV21_NV61_NV42;
        break;

    //422 formats
    case kSoftwareFormatI422   :
    shader_string += SHADER_BODY_I420_I422_I444;
        break;
    case kSoftwareFormatNV16   :
    shader_string += SHADER_BODY_NV12_NV16_NV24;
        break;
    case kSoftwareFormatNV61   :
        shader_string += SHADER_BODY_NV21_NV61_NV42;
        break;
    case kSoftwareFormatYUYV422   :
        shader_string += SHADER_BODY_YUYV422;
        break;
    case kSoftwareFormatYVYU422   :
        shader_string += SHADER_BODY_YVYU422;
        break;
    case kSoftwareFormatUYVY422   :
        shader_string += SHADER_BODY_UYVY422;
        break;

    //444 formats
    case kSoftwareFormatI444   :
        shader_string += SHADER_BODY_I420_I422_I444;
        break;
    case kSoftwareFormatNV24      :
        shader_string += SHADER_BODY_NV12_NV16_NV24;
        break;
    case kSoftwareFormatNV42      :
        shader_string += SHADER_BODY_NV21_NV61_NV42;
        break;
    case kSoftwareFormatYUV444    :
        shader_string += SHADER_BODY_YUV444;
        break;

    //rgb/gbr
    case kSoftwareFormatRGB24     :
        shader_string += SHADER_BODY_RGB;
        break;
    case kSoftwareFormatBGR24     :
        shader_string += SHADER_BODY_BGR;
        break;

    //rgba formats
    case kSoftwareFormatRGBA32    :
        shader_string += SHADER_BODY_RGBA;
        break;
    case kSoftwareFormatBGRA32    :
        if(bgra_support_)
            shader_string += SHADER_BODY_BGRA_IN;
        else
            shader_string += SHADER_BODY_BGRA;
        break;
    case kSoftwareFormatARGB32    :
        shader_string += SHADER_BODY_ARGB;
        break;
    case kSoftwareFormatABGR32    :
        shader_string += SHADER_BODY_ABGR;
        break;

    //gray
    case kSoftwareFormatGRAY8     :
        shader_string += SHADER_BODY_GRAY;
        break;

    //gray alpha
    case kSoftwareFormatGRAY8A    :
        shader_string += SHADER_BODY_GRAY_ALPHA;
        break;
    default:
        break;
    }

    return shader_string;
}


void TextureGenericOpenGL::get_capability()
{
    auto string_get_first_number = [](const char* text)->double{
        int32_t len = strlen(text);
        for(int index = 0; index < len ; index++){
            char ch = text[index];
            if( ch > '0' && ch <= '9'){
                return atof(text + index);
            }
        }
        return 0;
    };
    if(version_ == 0){
        const char* version = (const char*)glGetString(GL_VERSION);
        VGFX_GL_CHECK("TextureGenericOpenGL::get_capability glGetString(GL_VERSION)")
        if(version){
            if(strstr(version,"OpenGL ES ") != nullptr){
                gles_ = true;
            }
            //get a better context version for gl version
            double gl_version = string_get_first_number(version);
            if(gl_version > 0)
                version_ = gl_version;
        }

        if(version_ < 3.0){
            auto extensions = (const char*)glGetString(GL_EXTENSIONS);
            VGFX_GL_CHECK("TextureGenericOpenGL::get_capability glGetString(GL_EXTENSIONS)")

            if(extensions && strstr(extensions,"GL_EXT_texture_format_BGRA8888"))
                bgra_support_ = true;

            std::istringstream stream(extensions);
            std::string s;
            while (getline(stream, s, ' ')) {
                extensions_.push_back(s);
            }
        }
        else{
            //glGetStringi come from opengl 3.0 and opengl es 3.0
            GLint count = 0;
            glGetIntegerv(GL_NUM_EXTENSIONS, &count);
            VGFX_GL_CHECK("TextureGenericOpenGL::get_capability glGetIntegerv(GL_NUM_EXTENSIONS)")

            for (GLint i = 0; i < count; i++) {
                auto extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
                VGFX_GL_CHECK("TextureGenericOpenGL::get_capability glGetStringi(GL_EXTENSIONS)")
                if(strcmp(extension, "GL_EXT_texture_format_BGRA8888") == 0){
                    bgra_support_ = true;
                }
                extensions_.push_back(extension);
            }
        }
    }
}
