#include "convert_from_i422.h"
#include "convert_manager.h"
void ConvertFromI422::register_converter()
{

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatI420,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        libyuv::I422ToI420(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                dest.data[2], dest.line_size[2],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatYV12,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{
        libyuv::I422ToI420(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                dest.data[2], dest.line_size[2],
                dest.data[1], dest.line_size[1],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatNV12,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{
        libyuv::I422ToNV21(source.data[0], source.line_size[0],
                source.data[2], source.line_size[2],
                source.data[1], source.line_size[1],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatNV21,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        libyuv::I422ToNV21(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatI422,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{
        libyuv::I422Copy(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                dest.data[2], dest.line_size[2],
                source.width, source.height);
        return  0;
    });

    Converter i422_to_nv16_nv61 = [](const SoftwareFrame& source,
                                      SoftwareFrame& dest,
                                      RotationMode rotate,
                                      const CropArea& crop_area) -> int32_t {
        int8_t u_plane_index = (dest.format == kSoftwareFormatNV16) ? 1 : 2;
        int8_t v_plane_index = (dest.format == kSoftwareFormatNV16) ? 2 : 1;

        libyuv::CopyPlane(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width, source.height);

        libyuv::MergeUVPlane(source.data[u_plane_index], source.line_size[u_plane_index],
                source.data[v_plane_index], source.line_size[v_plane_index],
                dest.data[1], dest.line_size[1],
                source.width/2, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatI422, kSoftwareFormatNV16, i422_to_nv16_nv61);
    ConvertManager::add_converter(kSoftwareFormatI422, kSoftwareFormatNV61, i422_to_nv16_nv61);

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatYUYV422,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        libyuv::I422ToYUY2(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatYVYU422,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        libyuv::I422ToYUY2(source.data[0], source.line_size[0],
                source.data[2], source.line_size[2],
                source.data[1], source.line_size[1],
                dest.data[0], dest.line_size[0],
                source.width, source.height);

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatUYVY422,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        libyuv::I422ToUYVY(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatI444,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        libyuv::I422ToI444(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                dest.data[2], dest.line_size[2],
                source.width, source.height);

        return  0;
    });

    Converter i422_to_nv24_nv42 = [](const SoftwareFrame& source,
                                      SoftwareFrame& dest,
                                      RotationMode rotate,
                                      const CropArea& crop_area) -> int32_t {
        int8_t u_plane_index = (dest.format == kSoftwareFormatNV24) ? 1 : 2;
        int8_t v_plane_index = (dest.format == kSoftwareFormatNV24) ? 2 : 1;

        SoftwareFrameWithMemory i444 = ConvertManager::thread_temporary_frame(kSoftwareFormatI444, source.width, source.height);


        libyuv::I422ToI444(source.data[0], source.line_size[0],
            source.data[1], source.line_size[1],
            source.data[2], source.line_size[2],
            dest.data[0], dest.line_size[0],
            i444.data[1], i444.line_size[1],
            i444.data[2], i444.line_size[2],
            source.width, source.height);

        libyuv::MergeUVPlane(i444.data[u_plane_index], i444.line_size[u_plane_index],
            i444.data[v_plane_index], i444.line_size[v_plane_index],
            dest.data[1], dest.line_size[1],
            source.width, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatI422, kSoftwareFormatNV24, i422_to_nv24_nv42);
    ConvertManager::add_converter(kSoftwareFormatI422, kSoftwareFormatNV42, i422_to_nv24_nv42);

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatYUV444,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        SoftwareFrameWithMemory i444 = ConvertManager::thread_temporary_frame(kSoftwareFormatI444, source.width, source.height );


        libyuv::I422ToI444(source.data[0], source.line_size[0],
            source.data[1], source.line_size[1],
            source.data[2], source.line_size[2],
            i444.data[0], i444.line_size[0],
            i444.data[1], i444.line_size[1],
            i444.data[2], i444.line_size[2],
            source.width, source.height);

        libyuv::MergeRGBPlane(i444.data[0], i444.line_size[0],
            i444.data[1], i444.line_size[1],
            i444.data[2], i444.line_size[2],
            dest.data[0], dest.line_size[0],
            source.width, source.height);

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatRGB24,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        libyuv::I422ToRAW(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatBGR24,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        libyuv::I422ToRGB24(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatRGBA32,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        libyuv::I422ToABGR(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatBGRA32,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        libyuv::I422ToARGB(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatARGB32,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        libyuv::I422ToBGRA(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatABGR32,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        libyuv::I422ToRGBA(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatGRAY8,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{

        libyuv::CopyPlane(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI422,kSoftwareFormatGRAY8A,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest,
                  RotationMode rotate,
                  const CropArea& crop_area)->int32_t{
        SoftwareFrameWithMemory alpha = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width, source.height);

        libyuv::SetPlane(alpha.data[0], alpha.line_size[0], source.width, source.height, 255);

        libyuv::MergeUVPlane(source.data[0], source.line_size[0],
            alpha.data[0], alpha.line_size[0],
            dest.data[0], dest.line_size[0],
            source.width, source.height);
        return  0;
    });

}
