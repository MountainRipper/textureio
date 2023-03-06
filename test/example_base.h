#ifndef EXAMPLE_BASE_H
#define EXAMPLE_BASE_H
#include <cstdint>
#include <string>

#define VGFX_GL_CHECK(level, format, ...) \
{\
    GLenum gl_error = GL_NO_ERROR;\
    while(GLenum err = glGetError()){\
        MP_LOG_MORE(level,"OpenGL Call Failed:{} {}",err,mp::fmt_format(format,##__VA_ARGS__));\
        gl_error = err;\
    }\
}


void logEglError(const char *name);
uint32_t create_texture(int width,int height,uint8_t value);
uint32_t create_program(const char* vss, const char* fss);
uint32_t print_shader_compile_info(uint32_t shader);

class ExampleBase{
public:
    virtual ~ExampleBase(){}
    virtual int32_t on_init(void *window) = 0;
    virtual int32_t on_deinit() = 0;
    virtual int32_t on_frame() = 0;


    virtual void button_callback( int bt, int action, int mods) = 0;
    virtual void cursor_callback( double x, double y) = 0;
    virtual void key_callback( int key, int scancode, int action, int mods) = 0;
    virtual void char_callback( unsigned int key) = 0;
    virtual void error_callback(int err, const char* desc) = 0;
    virtual void resize_callback( int width, int height) = 0;
    virtual void scroll_callback( double xoffset, double yoffset) = 0;

    virtual void command(std::string command) = 0;
};

#endif // EXAMPLE_BASE_H
