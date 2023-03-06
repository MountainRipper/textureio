#include <cstring>
#include <sstream>
#include <stdio.h>
#include <tio/tio_hardware_graphic.h>
#include <vector>
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
    #ifndef GL_LUMINANCE
    #define GL_LUMINANCE 0x1909
    #endif
    #ifndef GL_LUMINANCE_ALPHA
    #define GL_LUMINANCE_ALPHA 0x190A
#endif
#elif __unix__
    #include <glad/gl.h>
#elif __posix

#endif

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
const char* SHADER_JPEG_CONSTANTS = R"(
const vec3 offset = vec3(0, -0.501960814, -0.501960814);
const mat3 matrix = mat3( 1,       1,        1,
                          0,      -0.3441,   1.772,
                          1.402,  -0.7141,   0);
)";

const char* SHADER_BT601_CONSTANTS = R"(
const vec3 offset = vec3(-0.0627451017, -0.501960814, -0.501960814);
const mat3 matrix = mat3( 1.1644,  1.1644,   1.1644,
                          0,      -0.3918,   2.0172,
                          1.596,  -0.813,    0);
)";

const char* SHADER_BT709_CONSTANTS = R"(
const vec3 offset = vec3(-0.0627451017, -0.501960814, -0.501960814);
const mat3 matrix = mat3( 1.1644,  1.1644,   1.1644,
                          0,      -0.2132,   2.1124,
                          1.7927, -0.5329,   0);
)";


const char* SHADER_BODY_YUV420P_444P = R"(
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

const char* SHADER_BODY_NV12_NV16_NV24_422P = R"(
void main()
{
    MEDIUMP vec3 yuv;
    LOWP vec3 rgb;

    yuv.x = TEXTURE2D(tex1,   v_texCoord).r;
    yuv.yz = TEXTURE2D(tex2, v_texCoord).ra;

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
    yuv.yz = TEXTURE2D(tex2, v_texCoord).ar;

    yuv += offset;
    rgb = matrix * yuv;

    gl_FragColor = vec4(rgb, 1);
    //__SHADER_BODY_REPLACER__
}
)";

const char* SHADER_BODY_YUYV422 = R"(
void main()
{
    MEDIUMP vec3 yuv;
    LOWP vec3 rgb;

    yuv.x = TEXTURE2D(tex1,   v_texCoord).r;
    //TODO
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
    vec2 ra = TEXTURE2D(tex1, v_texCoord).ra;
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
    SHADER_BT709_CONSTANTS,
    SHADER_BT709_CONSTANTS,//fix this
    SHADER_BT709_CONSTANTS,//fix this
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
int32_t TextureGenericOpenGL::upload(const SoftwareFrame &frame, GraphicTexture &texture)
{
    get_capability();
    const SoftwareFormatPlaner& planers = *TextureIO::planers_of_software_frame(frame.format);

    const GLuint channel_foramt_core[5] = {0,GL_RED,GL_RG,GL_RGB8,GL_RGBA8};
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

        if(frame.format == kSoftwareFrameBGRA32 && bgra_support_){
            format = GL_BGRA;
        }

        glActiveTexture(texture.flags[index]);
        VGFX_GL_CHECK("TextureGenericOpenGL::upload glActiveTexture")

        glPixelStorei(GL_UNPACK_ALIGNMENT, _tio_max_align(linesize));
        VGFX_GL_CHECK("TextureGenericOpenGL::upload glPixelStorei(GL_UNPACK_ALIGNMENT)")

        glPixelStorei(GL_UNPACK_ROW_LENGTH,linesize);
        VGFX_GL_CHECK("TextureGenericOpenGL::upload glPixelStorei(GL_UNPACK_ROW_LENGTH)")

        glBindTexture(GL_TEXTURE_2D, texture.context[index]);
        VGFX_GL_CHECK("TextureGenericOpenGL::upload glBindTexture")

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

    return 0;
}

std::string TextureGenericOpenGL::reference_shader_software(SoftwareFrameFormat format,YuvColorSpace color_space)
{
    get_capability();
    const SoftwareFormatPlaner& planers = *TextureIO::planers_of_software_frame(format);

    std::string shader_string = SHADER_HEADER_DEFINE_PRECISION_AND_TEXCOORD;

    shader_string += sharder_textures_define_of_count[planers.count];

    if(format >= kSoftwareFrameYUVStart && format <= kSoftwareFrameYUVEnd){
        shader_string += sharder_colorspace_define_of_type[color_space];
    }

    switch (format) {
    //420 formats
    case kSoftwareFrameI420:
        shader_string += SHADER_BODY_YUV420P_444P;
        break;
    case kSoftwareFrameYV12:
        shader_string += SHADER_BODY_YV12;
        break;
    case kSoftwareFrameNV12:
        shader_string += SHADER_BODY_NV12_NV16_NV24_422P;
        break;///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    case kSoftwareFrameNV21:
        shader_string += SHADER_BODY_NV21_NV61_NV42;
        break;

    //422 formats
    case kSoftwareFrameI422   :
        shader_string += SHADER_BODY_YUV420P_444P;
        break;
    case kSoftwareFrameNV16   :
        shader_string += SHADER_BODY_NV12_NV16_NV24_422P;
        break;
    case kSoftwareFrameNV61   :
        shader_string += SHADER_BODY_NV21_NV61_NV42;
        break;
    case kSoftwareFrameYUYV422   : break;
    case kSoftwareFrameYVYU422   : break;
    case kSoftwareFrameUYVY422   : break;

    //444 formats
    case kSoftwareFrameI444   :
        shader_string += SHADER_BODY_YUV420P_444P;
        break;
    case kSoftwareFrameNV24      :
        shader_string += SHADER_BODY_NV12_NV16_NV24_422P;
        break;
    case kSoftwareFrameNV42      :
        shader_string += SHADER_BODY_NV21_NV61_NV42;
        break;
    case kSoftwareFrameYUV444    :
        shader_string += SHADER_BODY_YUV444;
        break;

    //rgb/gbr
    case kSoftwareFrameRGB24     :
        shader_string += SHADER_BODY_RGB;
        break;
    case kSoftwareFrameBGR24     :
        shader_string += SHADER_BODY_BGR;
        break;

    //rgba formats
    case kSoftwareFrameRGBA32    :
        shader_string += SHADER_BODY_RGBA;
        break;
    case kSoftwareFrameBGRA32    :
        if(bgra_support_)
            shader_string += SHADER_BODY_BGRA_IN;
        else
            shader_string += SHADER_BODY_BGRA;
        break;
    case kSoftwareFrameARGB32    :
        shader_string += SHADER_BODY_ARGB;
        break;
    case kSoftwareFrameABGR32    :
        shader_string += SHADER_BODY_ABGR;
        break;

    //gray
    case kSoftwareFrameGRAY8     :
        shader_string += SHADER_BODY_GRAY;
        break;

    //gray alpha
    case kSoftwareFrameGRAY8A    :
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
