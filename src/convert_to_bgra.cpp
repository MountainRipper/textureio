#include "convert_to_bgra.h"
#include "convert_manager.h"
void ConvertToBGRA::register_converter()
{
    Converter convert_to_argb = [](
                                    const SoftwareFrame& source,
                                    SoftwareFrame& dest,
                                    RotationMode rotate,
                                    const CropArea& crop_area) -> int32_t {
        auto source_fourcc = g_software_format_maps[source.format];
        auto dest_fourcc = g_software_format_maps[dest.format];
        int32_t crop_width = (crop_area.crop_width == UINT32_MAX) ? source.width : crop_area.crop_width;
        int32_t crop_height = (crop_area.crop_height == UINT32_MAX) ? source.height : crop_area.crop_height;

        libyuv::ConvertToARGB(source.data[0], 0,
            dest.data[0], dest.line_size[0],
            crop_area.crop_x, crop_area.crop_y,
            source.line_size[0] / source_fourcc.stride_unit_0, source.height,
            crop_width, crop_height,
            libyuv::kRotate0,
            source_fourcc.fourcc);
        return 0;
    };

    for (int format_index = kSoftwareFormatFirst; format_index < kSoftwareFormatCount; format_index++) {
        if (g_software_format_maps[format_index].fourcc == libyuv::FOURCC_ANY)
            continue;
        ConvertManager::add_converter(static_cast<SoftwareFrameFormat>(format_index), kSoftwareFormatBGRA32, convert_to_argb);
    }

    Converter nv16_nv61_to_argb = [](
                                      const SoftwareFrame& source,
                                      SoftwareFrame& dest,
                                      RotationMode rotate,
                                      const CropArea& crop_area) -> int32_t {
        int8_t u_plane_index = (source.format == kSoftwareFormatNV16) ? 1 : 2;
        int8_t v_plane_index = (source.format == kSoftwareFormatNV16) ? 2 : 1;

        SoftwareFrameWithMemory i422 = ConvertManager::thread_temporary_frame(kSoftwareFormatI422, source.width, source.height);

        libyuv::SplitUVPlane(source.data[1], source.line_size[1],
            i422.data[u_plane_index], i422.line_size[u_plane_index],
            i422.data[v_plane_index], i422.line_size[v_plane_index],
            source.width / 2, source.height);

        libyuv::I422ToARGB(source.data[0], source.line_size[0],
            i422.data[1], i422.line_size[1],
            i422.data[2], i422.line_size[2],
            dest.data[0], dest.line_size[0],
            source.width, source.height);

        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV16, kSoftwareFormatBGRA32, nv16_nv61_to_argb);
    ConvertManager::add_converter(kSoftwareFormatNV61, kSoftwareFormatBGRA32, nv16_nv61_to_argb);

    ConvertManager::add_converter(kSoftwareFormatYVYU422, kSoftwareFormatBGRA32,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            SoftwareFrameWithMemory i422 = ConvertManager::thread_temporary_frame(kSoftwareFormatI422, source.width, source.height);

            libyuv::YUY2ToI422(source.data[0], source.line_size[0],
                    i422.data[0], i422.line_size[0],
                    i422.data[2], i422.line_size[2],
                    i422.data[1], i422.line_size[1],
                    source.width, source.height);

            libyuv::I422ToARGB(i422.data[0], i422.line_size[0],
                    i422.data[1], i422.line_size[1],
                    i422.data[2], i422.line_size[2],
                    dest.data[0], dest.line_size[0],
                    source.width, source.height);
            return 0;
        });

    Converter nv24_nv42_to_bgra = [](
                                      const SoftwareFrame& source,
                                      SoftwareFrame& dest,
                                      RotationMode rotate,
                                      const CropArea& crop_area) -> int32_t {
        int8_t u_plane_index = (source.format == kSoftwareFormatNV24) ? 1 : 2;
        int8_t v_plane_index = (source.format == kSoftwareFormatNV24) ? 2 : 1;

        SoftwareFrameWithMemory i444 = ConvertManager::thread_temporary_frame(kSoftwareFormatI444, source.width, source.height);

        libyuv::SplitUVPlane(source.data[1], source.line_size[1],
            i444.data[u_plane_index], i444.line_size[u_plane_index],
            i444.data[v_plane_index], i444.line_size[v_plane_index],
            source.width, source.height);
        libyuv::CopyPlane(source.data[0], source.line_size[0],
            i444.data[0], i444.line_size[0],
            source.width, source.height);

        libyuv::I444ToARGB(i444.data[0], i444.line_size[0],
            i444.data[1], i444.line_size[1],
            i444.data[2], i444.line_size[2],
            dest.data[0], dest.line_size[0],
            source.width, source.height);

        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV24, kSoftwareFormatBGRA32, nv24_nv42_to_bgra);
    ConvertManager::add_converter(kSoftwareFormatNV42, kSoftwareFormatBGRA32, nv24_nv42_to_bgra);

    ConvertManager::add_converter(kSoftwareFormatYUV444, kSoftwareFormatBGRA32,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            SoftwareFrameWithMemory i444 = ConvertManager::thread_temporary_frame(kSoftwareFormatI444, source.width, source.height);

            libyuv::SplitRGBPlane(source.data[0], source.line_size[0],
                i444.data[0], i444.line_size[0],
                i444.data[1], i444.line_size[1],
                i444.data[2], i444.line_size[2],
                source.width, source.height);

            libyuv::I444ToARGB(i444.data[0], i444.line_size[0],
                i444.data[1], i444.line_size[1],
                i444.data[2], i444.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatGRAY8, kSoftwareFormatBGRA32,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I400ToARGB(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
            return 0;
        });
    ConvertManager::add_converter(kSoftwareFormatGRAY8A, kSoftwareFormatBGRA32,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            SoftwareFrameWithMemory gray = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width, source.height);
            SoftwareFrameWithMemory alpha = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width, source.height,1);

            libyuv::SplitUVPlane(source.data[0], source.line_size[0],
                gray.data[0], gray.line_size[0],
                alpha.data[0], alpha.line_size[0],
                source.width, source.height);

            libyuv::I400ToARGB(gray.data[0], gray.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width, source.height);

            libyuv::ARGBCopyYToAlpha(alpha.data[0], alpha.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
            return 0;
        });
}
