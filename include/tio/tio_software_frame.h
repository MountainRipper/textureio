#ifndef RAWFRAMECONVERT_H
#define RAWFRAMECONVERT_H
#include "tio_types.h"

namespace mr {
namespace tio {

class SoftwareFrameConvert
{
public:
    SoftwareFrameConvert();

    int32_t convert(const SoftwareFrame& source,SoftwareFrame& dest,RotationMode rotate = kRotate0,const CropArea& crop_area=CropArea());
};

}//namespace tio
}//namespace mountain-ripper


#endif // RAWFRAMECONVERT_H
