#include <logger.h>
#include <glad/egl.h>
#include <glad/gl.h>
#include "example_base.h"


void logEglError(const char *name) {
    const char *err;
    switch (eglGetError()) {
    case EGL_NOT_INITIALIZED:
        err = "EGL_NOT_INITIALIZED";
        break;
    case EGL_BAD_ACCESS:
        err = "EGL_BAD_ACCESS";
        break;
    case EGL_BAD_ALLOC:
        err = "EGL_BAD_ALLOC";
        break;
    case EGL_BAD_ATTRIBUTE:
        err = "EGL_BAD_ATTRIBUTE";
        break;
    case EGL_BAD_CONTEXT:
        err = "EGL_BAD_CONTEXT";
        break;
    case EGL_BAD_CONFIG:
        err = "EGL_BAD_CONFIG";
        break;
    case EGL_BAD_CURRENT_SURFACE:
        err = "EGL_BAD_CURRENT_SURFACE";
        break;
    case EGL_BAD_DISPLAY:
        err = "EGL_BAD_DISPLAY";
        break;
    case EGL_BAD_SURFACE:
        err = "EGL_BAD_SURFACE";
        break;
    case EGL_BAD_MATCH:
        err = "EGL_BAD_MATCH";
        break;
    case EGL_BAD_PARAMETER:
        err = "EGL_BAD_PARAMETER";
        break;
    case EGL_BAD_NATIVE_PIXMAP:
        err = "EGL_BAD_NATIVE_PIXMAP";
        break;
    case EGL_BAD_NATIVE_WINDOW:
        err = "EGL_BAD_NATIVE_WINDOW";
        break;
    case EGL_CONTEXT_LOST:
        err = "EGL_CONTEXT_LOST";
        break;
    default:
        err = "unknown";
        break;
    }
    MP_ERROR("{} failed with: {}",name,err);
}



uint32_t create_texture(int width,int height,uint8_t value){
    GLuint target_texture = 0;
    glGenTextures(1,&target_texture);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"glGenTextures");
    glBindTexture(GL_TEXTURE_2D, target_texture);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"glBindTexture");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"GL_TEXTURE_MIN_FILTER");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"GL_TEXTURE_MAG_FILTER");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"GL_TEXTURE_WRAP_S");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"GL_TEXTURE_WRAP_T");

    uint8_t* image = new uint8_t[width* height*4];
    memset(image,value,width* height*4);
    glTexImage2D(GL_TEXTURE_2D, 0,
               GL_RGBA,
               width, height, 0,
               GL_RGBA, GL_UNSIGNED_BYTE,
               image);
    delete [] image;
    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"glTexImage2D");
    return target_texture;
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

uint32_t create_program(const char* vss, const char* fss){

    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"glCreateShader before");
    auto vert = glCreateShader(GL_VERTEX_SHADER);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"glCreateShader for vertex shader");
    glShaderSource(vert, 1, &vss, nullptr);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"glShaderSource for vertex shader");
    glCompileShader(vert);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"glCompileShader for vertex shader");
    print_shader_compile_info(vert);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"print_shader_compile_info for vertex shader");

    auto frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fss, nullptr);
    glCompileShader(frag);
    print_shader_compile_info(frag);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"glCreateShader for fragment shader");

    auto prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError,"create program");
    return prog;
}
