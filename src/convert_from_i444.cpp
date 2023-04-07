#include "convert_from_i444.h"
#include "convert_manager.h"
void ConvertFromI444::register_converter()
{

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatI420,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{


        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatYV12,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatNV12,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatNV21,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatI422,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatNV16,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatNV61,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatYUYV422,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatYVYU422,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatUYVY422,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatI444,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatNV24,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatNV42,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatYUV444,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatRGB24,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatBGR24,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatRGBA32,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatBGRA32,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatARGB32,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatABGR32,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatGRAY8,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatGRAY8A,
                  [](SoftwareFrameWithMemory& source,
                  SoftwareFrameWithMemory& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        return  0;
    });

}
