#ifndef RAWFRAMECONVERT_H
#define RAWFRAMECONVERT_H
#include "tio_types.h"

namespace mr {
namespace tio {

class SoftwareFrameConvert
{
public:
    static int32_t convert(const SoftwareFrame& source,SoftwareFrame& dest,RotationMode rotate = kRotate0,FillMode fill_mode = kStretchFill);
};

class SoftwareFramePackerPrivate;
class SoftwareFramePacker{
public:
    int32_t create(int32_t item_size_wider = 0,FillMode fill_mode = kAspectCrop,int32_t max_output_size=4096, bool pow_of_two = false);
    int32_t add(const SoftwareFrame& frame,const std::string& name,bool keep_aspect_ratio);
    int32_t add_origin(const SoftwareFrame& frame,const std::string& name);
    int32_t finish(const std::string &filename);
private:
    std::shared_ptr<SoftwareFramePackerPrivate> context_;
};
}//namespace tio
}//namespace mountain-ripper


#endif // RAWFRAMECONVERT_H
