#ifndef TEXTUREIOEXAMPLE_H
#define TEXTUREIOEXAMPLE_H
#include <mrcommon/sdl3_runner.h>
#include <tio/tio_hardware_graphic.h>
#include <tio/tio_software_frame.h>

using namespace mr::tio;
class TextureioExample : public SDLShowcaseBase
{
public:
    TextureioExample();

    // ExampleBase interface
public:
    virtual int32_t on_set_params(cxxopts::Options& options) override;
    virtual int32_t on_pre_init(cxxopts::ParseResult& options_result,uint32_t& window_flags) override;
    virtual int32_t on_init(void *window,int width, int height) override;
    virtual int32_t on_deinit() override;
    virtual int32_t on_frame() override;
    virtual void button_callback(int bt,int type,int clicks,double x,double y) override;
    virtual void cursor_callback(double x, double y) override;
    virtual void key_callback(int key, int type,int scancode,int mods) override;
    virtual void error_callback(int err, const char *desc) override;
    virtual void resize_callback(int width, int height) override;
    virtual void command(std::string command) override;
private:
    void convert_to_target_frames();
    void create_programs();
private:
    uint32_t textures_[4] = {0};
    uint32_t texture_unit_base_ = 2;
    std::shared_ptr<ReferenceShader> programs_[kSoftwareFormatCount] = {0};

    int width_ = 0;
    int height_ = 0;

    SoftwareFrameConvert software_converter_;
    SoftwareFrameWithMemory origin_image_[2];
    SoftwareFrameWithMemory source_image_;
    std::shared_ptr<SoftwareFrameWithMemory> target_frames_[kSoftwareFormatCount];
    float convert_ms_[kSoftwareFormatCount] = {0};
    uint32_t frames_convert_count_ = 0;
    const char* format_names[kSoftwareFormatCount] = {0};

    int source_format_ = kSoftwareFormatBGRA32;
    int sampler_mode_ = kSamplerAuto;
    int colorspace_ = kColorSpaceBt601;
    int rotate_ = kRotate0;

    float crop_aspect_ratio_ = 0;
    int convert_fill_mode_ = 0;
    int render_fill_mode_ = -1;

    float render_scale_x_ = 1;
    float render_scale_y_ = 1;
    float render_offset_x_ = 0;
    float render_offset_y_ = 0;
    float render_rotate_ = 0;

    int origin_image_use_ = 0;
    bool show_source_ = false;

    bool resized_ = false;
    FrameArea final_size_;
};



#endif // TEXTUREIOEXAMPLE_H
