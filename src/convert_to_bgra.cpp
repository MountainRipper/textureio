#include "convert_to_bgra.h"
#include "convert_manager.h"
#include <math.h>
void ConvertToBGRA::register_converter()
{
    Converter convert_to_argb = [](const SoftwareFrame& source,
                                    SoftwareFrame& dest) -> int32_t {
        auto source_fourcc = g_software_format_maps[source.format];
        auto dest_fourcc = g_software_format_maps[dest.format];
        libyuv::ConvertToARGB(source.data[0], 0,
                dest.data[0], dest.linesize[0],
            0, 0,
                source.linesize[0] / source_fourcc.stride_unit_0, source.height,
            source.width, source.height,
            libyuv::kRotate0,
            source_fourcc.fourcc);
        return 0;
    };

    for (int format_index = kSoftwareFormatFirst; format_index < kSoftwareFormatCount; format_index++) {
        if (g_software_format_maps[format_index].fourcc == libyuv::FOURCC_ANY)
            continue;
        ConvertManager::add_converter(static_cast<SoftwareFrameFormat>(format_index), kSoftwareFormatBGRA32, convert_to_argb);
    }

    //use direct NV12ToARGB ,so can use special linesize
    ConvertManager::add_converter(kSoftwareFormatNV12, kSoftwareFormatBGRA32, [](
                                      const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {
        int8_t u_plane_index = (source.format == kSoftwareFormatNV12) ? 1 : 2;
        int8_t v_plane_index = (source.format == kSoftwareFormatNV12) ? 2 : 1;

        libyuv::NV12ToARGB(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                dest.data[0], dest.linesize[0],
            source.width, source.height);

        return 0;
    });
    ConvertManager::add_converter(kSoftwareFormatNV21, kSoftwareFormatBGRA32, [](
                                      const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {
        int8_t u_plane_index = (source.format == kSoftwareFormatNV12) ? 1 : 2;
        int8_t v_plane_index = (source.format == kSoftwareFormatNV12) ? 2 : 1;

        libyuv::NV21ToARGB(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                dest.data[0], dest.linesize[0],
            source.width, source.height);

        return 0;
    });

    Converter nv16_nv61_to_argb = [](
                                      const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {
        int8_t u_plane_index = (source.format == kSoftwareFormatNV16) ? 1 : 2;
        int8_t v_plane_index = (source.format == kSoftwareFormatNV16) ? 2 : 1;

        SoftwareFrameWithMemory i422 = ConvertManager::thread_temporary_frame(kSoftwareFormatI422, source.width, source.height);

        libyuv::SplitUVPlane(source.data[1], source.linesize[1],
                i422.data[u_plane_index], i422.linesize[u_plane_index],
                i422.data[v_plane_index], i422.linesize[v_plane_index],
            ceil(source.width / 2.0), source.height);

        libyuv::I422ToARGB(source.data[0], source.linesize[0],
                i422.data[1], i422.linesize[1],
                i422.data[2], i422.linesize[2],
                dest.data[0], dest.linesize[0],
            source.width, source.height);

        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV16, kSoftwareFormatBGRA32, nv16_nv61_to_argb);
    ConvertManager::add_converter(kSoftwareFormatNV61, kSoftwareFormatBGRA32, nv16_nv61_to_argb);

    ConvertManager::add_converter(kSoftwareFormatYVYU422, kSoftwareFormatBGRA32,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
            SoftwareFrameWithMemory i422 = ConvertManager::thread_temporary_frame(kSoftwareFormatI422, source.width, source.height);

            libyuv::YUY2ToI422(source.data[0], source.linesize[0],
                    i422.data[0], i422.linesize[0],
                    i422.data[2], i422.linesize[2],
                    i422.data[1], i422.linesize[1],
                    source.width, source.height);

            libyuv::I422ToARGB(i422.data[0], i422.linesize[0],
                    i422.data[1], i422.linesize[1],
                    i422.data[2], i422.linesize[2],
                    dest.data[0], dest.linesize[0],
                    source.width, source.height);
            return 0;
        });

    Converter nv24_nv42_to_bgra = [](
                                      const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {
        int8_t u_plane_index = (source.format == kSoftwareFormatNV24) ? 1 : 2;
        int8_t v_plane_index = (source.format == kSoftwareFormatNV24) ? 2 : 1;

        SoftwareFrameWithMemory i444 = ConvertManager::thread_temporary_frame(kSoftwareFormatI444, source.width, source.height);

        libyuv::SplitUVPlane(source.data[1], source.linesize[1],
                i444.data[u_plane_index], i444.linesize[u_plane_index],
                i444.data[v_plane_index], i444.linesize[v_plane_index],
            source.width, source.height);
        libyuv::CopyPlane(source.data[0], source.linesize[0],
                i444.data[0], i444.linesize[0],
            source.width, source.height);

        libyuv::I444ToARGB(i444.data[0], i444.linesize[0],
                i444.data[1], i444.linesize[1],
                i444.data[2], i444.linesize[2],
                dest.data[0], dest.linesize[0],
            source.width, source.height);

        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV24, kSoftwareFormatBGRA32, nv24_nv42_to_bgra);
    ConvertManager::add_converter(kSoftwareFormatNV42, kSoftwareFormatBGRA32, nv24_nv42_to_bgra);

    ConvertManager::add_converter(kSoftwareFormatYUV444, kSoftwareFormatBGRA32,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
            SoftwareFrameWithMemory i444 = ConvertManager::thread_temporary_frame(kSoftwareFormatI444, source.width, source.height);

            libyuv::SplitRGBPlane(source.data[0], source.linesize[0],
                    i444.data[0], i444.linesize[0],
                    i444.data[1], i444.linesize[1],
                    i444.data[2], i444.linesize[2],
                source.width, source.height);

            libyuv::I444ToARGB(i444.data[0], i444.linesize[0],
                    i444.data[1], i444.linesize[1],
                    i444.data[2], i444.linesize[2],
                    dest.data[0], dest.linesize[0],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatGRAY8, kSoftwareFormatBGRA32,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
        libyuv::I400ToARGB(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                source.width, source.height);
            return 0;
        });
    ConvertManager::add_converter(kSoftwareFormatGRAY8A, kSoftwareFormatBGRA32,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
            SoftwareFrameWithMemory gray = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width, source.height);
            SoftwareFrameWithMemory alpha = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width, source.height,'a');

            libyuv::SplitUVPlane(source.data[0], source.linesize[0],
                    gray.data[0], gray.linesize[0],
                    alpha.data[0], alpha.linesize[0],
                source.width, source.height);

            libyuv::I400ToARGB(gray.data[0], gray.linesize[0],
                    dest.data[0], dest.linesize[0],
                source.width, source.height);

            libyuv::ARGBCopyYToAlpha(alpha.data[0], alpha.linesize[0],
                    dest.data[0], dest.linesize[0],
                source.width, source.height);
            return 0;
        });
}
