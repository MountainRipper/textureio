#ifndef TEXTUREIOEXAMPLE_H
#define TEXTUREIOEXAMPLE_H
#include "example_base.h"
#include "tio/tio_hardware_graphic.h"
#include "tio/tio_software_frame.h"

using namespace mr::tio;
class TextureioExample : public ExampleBase
{
public:
    TextureioExample();

    // ExampleBase interface
public:
    virtual int32_t on_init(void *window) override;
    virtual int32_t on_deinit() override;
    virtual int32_t on_frame() override;
    virtual void button_callback(int bt, int action, int mods) override;
    virtual void cursor_callback(double x, double y) override;
    virtual void key_callback(int key, int scancode, int action, int mods) override;
    virtual void char_callback(unsigned int key) override;
    virtual void error_callback(int err, const char *desc) override;
    virtual void resize_callback(int width, int height) override;
    virtual void scroll_callback(double xoffset, double yoffset) override;
    virtual void command(std::string command) override;
private:
    uint32_t textures_[4] = {0};
    uint32_t texture_locations_[4] = {0};
    uint32_t texture_unit_base_ = 2;
    uint32_t program_[kSoftwareFrameCount];
};



#endif // TEXTUREIOEXAMPLE_H
