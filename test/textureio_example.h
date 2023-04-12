#ifndef TEXTUREIOEXAMPLE_H
#define TEXTUREIOEXAMPLE_H
#include "example_base.h"
#include <tio/tio_hardware_graphic.h>
#include <tio/tio_software_frame.h>

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
    void convert_to_target_frames();
    void create_programs();
private:
    uint32_t textures_[4] = {0};
    uint32_t texture_unit_base_ = 2;
    uint32_t programs_[kSoftwareFormatCount] = {0};
    uint32_t g_vao = 0;
    int width_ = 0;
    int height_ = 0;

    SoftwareFrameConvert software_converter_;
    SoftwareFrameWithMemory origin_image_[2];
    SoftwareFrameWithMemory source_image_;
    std::shared_ptr<SoftwareFrameWithMemory> target_frames_[kSoftwareFormatCount];
    float convert_ms_[kSoftwareFormatCount] = {0};
    uint32_t frames_convert_count_ = 0;

    int source_format_ = kSoftwareFormatBGRA32;
    int sampler_mode_ = kSamplerAuto;
    int colorspace_ = kColorSpaceBt601;
    int origin_image_use_ = 0;
    bool show_source_ = false;

    bool resized_ = false;
};



#endif // TEXTUREIOEXAMPLE_H
