#include "convert_to_i422.h"
#include "convert_manager.h"

int32_t ConvertToI422::register_converter()
{

    Converter i420_yv12_to_i422 = [](
                                      const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {
        int8_t u_plane_index = (source.format == kSoftwareFormatI420) ? 1 : 2;
        int8_t v_plane_index = (source.format == kSoftwareFormatI420) ? 2 : 1;
        libyuv::I420ToI422(source.data[0], source.linesize[0],
                source.data[u_plane_index], source.linesize[u_plane_index],
                source.data[v_plane_index], source.linesize[v_plane_index],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                dest.data[2], dest.linesize[2],
            source.width, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatI422, i420_yv12_to_i422);
    ConvertManager::add_converter(kSoftwareFormatYV12, kSoftwareFormatI422, i420_yv12_to_i422);

    Converter nv12_nv21_to_i422 = [](
                                      const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {
        int8_t u_plane_index = (source.format == kSoftwareFormatNV12) ? 1 : 2;
        int8_t v_plane_index = (source.format == kSoftwareFormatNV12) ? 2 : 1;

        SoftwareFrameWithMemory i420 = ConvertManager::thread_temporary_frame(kSoftwareFormatI420, source.width, source.height);

        libyuv::SplitUVPlane(source.data[1], source.linesize[1],
                i420.data[u_plane_index], i420.linesize[u_plane_index],
                i420.data[v_plane_index], i420.linesize[v_plane_index],
            source.width / 2, source.height / 2);

        libyuv::I420ToI422(source.data[0], source.linesize[0],
                i420.data[1], i420.linesize[1],
                i420.data[2], i420.linesize[2],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                dest.data[2], dest.linesize[2],
            source.width, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV12, kSoftwareFormatI422, nv12_nv21_to_i422);
    ConvertManager::add_converter(kSoftwareFormatNV21, kSoftwareFormatI422, nv12_nv21_to_i422);

    ConvertManager::add_converter(kSoftwareFormatI422, kSoftwareFormatI422,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
        libyuv::I422Copy(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                source.data[2], source.linesize[2],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                dest.data[2], dest.linesize[2],
                source.width, source.height);
            return 0;
        });

    Converter nv16_nv61_to_i422 = [](
                                      const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {
        int8_t u_plane_index = (source.format == kSoftwareFormatNV16) ? 1 : 2;
        int8_t v_plane_index = (source.format == kSoftwareFormatNV16) ? 2 : 1;

        libyuv::SplitUVPlane(source.data[1], source.linesize[1],
                dest.data[u_plane_index], dest.linesize[u_plane_index],
                dest.data[v_plane_index], dest.linesize[v_plane_index],
            source.width / 2, source.height);
        libyuv::CopyPlane(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
            source.width, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV16, kSoftwareFormatI422, nv16_nv61_to_i422);
    ConvertManager::add_converter(kSoftwareFormatNV61, kSoftwareFormatI422, nv16_nv61_to_i422);

    ConvertManager::add_converter(kSoftwareFormatYUYV422, kSoftwareFormatI422,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
        libyuv::YUY2ToI422(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                dest.data[2], dest.linesize[2],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatYVYU422, kSoftwareFormatI422,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {

        libyuv::YUY2ToI422(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                dest.data[2], dest.linesize[2],
                dest.data[1], dest.linesize[1],
                source.width, source.height);
            return 0;
        });
    ConvertManager::add_converter(kSoftwareFormatUYVY422, kSoftwareFormatI422,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
        libyuv::UYVYToI422(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                dest.data[2], dest.linesize[2],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI444, kSoftwareFormatI422,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
        libyuv::CopyPlane(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                source.width, source.height);
            // half uv width of 444
        libyuv::ScalePlane(source.data[1], source.linesize[1], source.width, source.height,
                dest.data[1], dest.linesize[1], dest.width / 2, dest.height, libyuv::kFilterLinear);
        libyuv::ScalePlane(source.data[2], source.linesize[2], source.width, source.height,
                dest.data[2], dest.linesize[2], dest.width / 2, dest.height, libyuv::kFilterLinear);
            return 0;
        });

    Converter nv24_nv42_to_i422 = [](
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
                dest.data[0], dest.linesize[0],
            i444.width, i444.height);
        // half uv width of 444
        libyuv::ScalePlane(i444.data[1], i444.linesize[1], i444.width, i444.height,
                dest.data[1], dest.linesize[1], dest.width / 2, dest.height, libyuv::kFilterLinear);
        libyuv::ScalePlane(i444.data[2], i444.linesize[2], i444.width, i444.height,
                dest.data[2], dest.linesize[2], dest.width / 2, dest.height, libyuv::kFilterLinear);

        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV24, kSoftwareFormatI422, nv24_nv42_to_i422);
    ConvertManager::add_converter(kSoftwareFormatNV42, kSoftwareFormatI422, nv24_nv42_to_i422);

    ConvertManager::add_converter(kSoftwareFormatYUV444, kSoftwareFormatI422,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
            SoftwareFrameWithMemory i444 = ConvertManager::thread_temporary_frame(kSoftwareFormatI444, source.width, source.height);

            libyuv::SplitRGBPlane(source.data[0], source.linesize[0],
                    i444.data[0], i444.linesize[0],
                    i444.data[1], i444.linesize[1],
                    i444.data[2], i444.linesize[2],
                source.width, source.height);

            libyuv::CopyPlane(i444.data[0], i444.linesize[0],
                    dest.data[0], dest.linesize[0],
                i444.width, i444.height);
            // half uv width of 444
            libyuv::ScalePlane(i444.data[1], i444.linesize[1], i444.width, i444.height,
                    dest.data[1], dest.linesize[1], dest.width / 2, dest.height, libyuv::kFilterLinear);
            libyuv::ScalePlane(i444.data[2], i444.linesize[2], i444.width, i444.height,
                    dest.data[2], dest.linesize[2], dest.width / 2, dest.height, libyuv::kFilterLinear);
            return 0;
        });
    // RGB 24 32 bits use argb intermediate format is best way current.
    ConvertManager::add_converter(kSoftwareFormatBGRA32, kSoftwareFormatI422,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
        libyuv::ARGBToI422(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                dest.data[2], dest.linesize[2],
                source.width, source.height);
            return 0;
        });

    Converter rgb24_a32_to_argb = [](const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {
        auto source_fourcc = g_software_format_maps[source.format];
        auto dest_fourcc = g_software_format_maps[dest.format];

        SoftwareFrameWithMemory bgra = ConvertManager::thread_temporary_frame(kSoftwareFormatBGRA32,source.width, source.height);

        libyuv::ConvertToARGB(source.data[0], 0,
                bgra.data[0], bgra.linesize[0],
            0, 0,
                source.linesize[0] / source_fourcc.stride_unit_0, source.height,
            source.width, source.height,
            libyuv::kRotate0,
            source_fourcc.fourcc);

        libyuv::ARGBToI422(bgra.data[0], bgra.linesize[0],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                dest.data[2], dest.linesize[2],
            source.width, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatRGB24, kSoftwareFormatI422, rgb24_a32_to_argb);
    ConvertManager::add_converter(kSoftwareFormatBGR24, kSoftwareFormatI422, rgb24_a32_to_argb);
    ConvertManager::add_converter(kSoftwareFormatABGR32, kSoftwareFormatI422, rgb24_a32_to_argb);
    ConvertManager::add_converter(kSoftwareFormatARGB32, kSoftwareFormatI422, rgb24_a32_to_argb);
    ConvertManager::add_converter(kSoftwareFormatRGBA32, kSoftwareFormatI422, rgb24_a32_to_argb);

    ConvertManager::add_converter(kSoftwareFormatGRAY8, kSoftwareFormatI422,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
        libyuv::CopyPlane(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                source.width, source.height);

        libyuv::SetPlane(dest.data[1], dest.linesize[1], dest.width / 2, dest.height, 128);
        libyuv::SetPlane(dest.data[2], dest.linesize[2], dest.width / 2, dest.height, 128);
            return 0;
        });
    ConvertManager::add_converter(kSoftwareFormatGRAY8A, kSoftwareFormatI422,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
            SoftwareFrameWithMemory alpha = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width, source.height);

            libyuv::SplitUVPlane(source.data[0], source.linesize[0],
                    dest.data[0], dest.linesize[0],
                    alpha.data[0], alpha.linesize[0],
                source.width, source.height);

            libyuv::SetPlane(dest.data[1], dest.linesize[1], dest.width / 2, dest.height, 128);
            libyuv::SetPlane(dest.data[2], dest.linesize[2], dest.width / 2, dest.height, 128);
            return 0;
        });
    return 0;
}
