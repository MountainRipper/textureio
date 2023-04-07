#include "convert_from_i422.h"
#include "convert_manager.h"
void ConvertFromI422::register_converter()
{

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatI420,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{


        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatYV12,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatNV12,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatNV21,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatI422,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatNV16,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatNV61,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatYUYV422,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatYVYU422,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatUYVY422,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatI444,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatNV24,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatNV42,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatYUV444,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatRGB24,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatBGR24,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatRGBA32,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatBGRA32,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatARGB32,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatABGR32,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatGRAY8,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatGRAY8A,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

}
