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

}//namespace tio
}//namespace mountain-ripper


#endif // RAWFRAMECONVERT_H
