#include "convert_from_bgra.h"
#include "convert_manager.h"
void ConvertFromBGRA::register_converter()
{

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatI420,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::ARGBToI420(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                dest.data[2], dest.line_size[2],
                source.width, source.height);

            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatYV12,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::ARGBToI420(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                dest.data[2], dest.line_size[2],
                dest.data[1], dest.line_size[1],
                source.width, source.height);

            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatNV12,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::ARGBToNV12(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatNV21,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::ARGBToNV21(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatI422,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::ARGBToI422(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                dest.data[2], dest.line_size[2],
                source.width, source.height);
            return 0;
        });

    Converter bgra_to_nv16_nv61 = [](const SoftwareFrame& source,
                                      SoftwareFrame& dest,
                                      RotationMode rotate,
                                      const CropArea& crop_area) -> int32_t {
        int8_t u_plane_index = (dest.format == kSoftwareFormatNV16) ? 1 : 2;
        int8_t v_plane_index = (dest.format == kSoftwareFormatNV16) ? 2 : 1;

        SoftwareFrameWithMemory i422 = ConvertManager::thread_temporary_frame(kSoftwareFormatI422, source.width, source.height );

        libyuv::ARGBToI422(source.data[0], source.line_size[0],
            dest.data[0], dest.line_size[0],
            i422.data[u_plane_index], i422.line_size[u_plane_index],
            i422.data[v_plane_index], i422.line_size[v_plane_index],
            source.width, source.height);

        libyuv::MergeUVPlane(i422.data[1], i422.line_size[1],
            i422.data[2], i422.line_size[2],
            dest.data[1], dest.line_size[1],
            source.width / 2, source.height);

        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatNV16, bgra_to_nv16_nv61);
    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatNV61, bgra_to_nv16_nv61);

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatYUYV422,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::ARGBToYUY2(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width, source.height);

            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatYVYU422,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            SoftwareFrameWithMemory i422 = ConvertManager::thread_temporary_frame(kSoftwareFormatI422, source.width, source.height);
            libyuv::ARGBToI422(source.data[0], source.line_size[0],
                    i422.data[0], i422.line_size[0],
                    i422.data[1], i422.line_size[1],
                    i422.data[2], i422.line_size[2],
                    source.width, source.height);

            libyuv::I422ToYUY2(i422.data[0], i422.line_size[0],
                    i422.data[2], i422.line_size[2],
                    i422.data[1], i422.line_size[1],
                    dest.data[0], dest.line_size[0],
                    source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatUYVY422,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::ARGBToUYVY(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatI444,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::ARGBToI444(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                dest.data[2], dest.line_size[2],
                source.width, source.height);
            return 0;
        });

    Converter bgra_to_nv24_nv42 = [](const SoftwareFrame& source,
                                      SoftwareFrame& dest,
                                      RotationMode rotate,
                                      const CropArea& crop_area) -> int32_t {
        int8_t u_plane_index = (dest.format == kSoftwareFormatNV24) ? 1 : 2;
        int8_t v_plane_index = (dest.format == kSoftwareFormatNV24) ? 2 : 1;

        SoftwareFrameWithMemory i444 = ConvertManager::thread_temporary_frame(kSoftwareFormatI444, source.width, source.height);

        libyuv::ARGBToI444(source.data[0], source.line_size[0],
            dest.data[0], dest.line_size[0],
            i444.data[u_plane_index], i444.line_size[u_plane_index],
            i444.data[v_plane_index], i444.line_size[v_plane_index],
            source.width, source.height);

        libyuv::MergeUVPlane(i444.data[1], i444.line_size[1],
            i444.data[2], i444.line_size[2],
            dest.data[1], dest.line_size[1],
            source.width, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatNV24, bgra_to_nv24_nv42);
    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatNV42, bgra_to_nv24_nv42);

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatYUV444,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {

            SoftwareFrameWithMemory i444 = ConvertManager::thread_temporary_frame(kSoftwareFormatI444, source.width, source.height);

            libyuv::ARGBToI444(source.data[0], source.line_size[0],
                i444.data[0], i444.line_size[0],
                i444.data[1], i444.line_size[1],
                i444.data[2], i444.line_size[2],
                source.width, source.height);

            libyuv::MergeRGBPlane(i444.data[0], i444.line_size[0],
                i444.data[1], i444.line_size[1],
                i444.data[2], i444.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatRGB24,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {

            libyuv::ARGBToRAW(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width, source.height);

            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatBGR24,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {

            libyuv::ARGBToRGB24(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width, source.height);

            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatRGBA32,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {

        libyuv::ARGBToABGR(source.data[0], source.line_size[0],
            dest.data[0], dest.line_size[0],
            source.width, source.height);

            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatBGRA32,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {

        libyuv::ARGBCopy(source.data[0], source.line_size[0],
            dest.data[0], dest.line_size[0],
            source.width, source.height);

            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatARGB32,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {

        libyuv::ARGBToBGRA(source.data[0], source.line_size[0],
            dest.data[0], dest.line_size[0],
            source.width, source.height);

            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatABGR32,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {

            libyuv::ARGBToRGBA(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width, source.height);

            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatGRAY8,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {

            libyuv::ARGBToI400(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width, source.height);

            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatGRAY8A,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {

            SoftwareFrameWithMemory gray = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width, source.height);
            SoftwareFrameWithMemory alpha = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width, source.height,1);

            libyuv::ARGBToI400(source.data[0], source.line_size[0],
                gray.data[0], gray.line_size[0],
                source.width, source.height);

            libyuv::ARGBExtractAlpha(source.data[0], source.line_size[0],
                    alpha.data[0], alpha.line_size[0],
                    source.width, source.height);

            libyuv::MergeUVPlane(gray.data[0], gray.line_size[0],
                    alpha.data[0], alpha.line_size[0],
                    dest.data[0], dest.line_size[0],
                    source.width, source.height);

            return 0;
        });
}
