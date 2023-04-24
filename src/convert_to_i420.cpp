#include "convert_to_i420.h"
#include "convert_manager.h"
void ConvertToI420::register_converter()
{

    Converter convert_to_i420 = [](
                                    const SoftwareFrame& source,
                                    SoftwareFrame& dest) -> int32_t {
        auto source_fourcc = g_software_format_maps[source.format];
        auto dest_fourcc = g_software_format_maps[dest.format];
        libyuv::ConvertToI420(source.data[0], 0,
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                dest.data[2], dest.linesize[2],
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
        ConvertManager::add_converter(static_cast<SoftwareFrameFormat>(format_index), kSoftwareFormatI420, convert_to_i420);
    }

    Converter nv16_nv61_to_i420 = [](
                                      const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {
        int8_t u_plane_index = (source.format == kSoftwareFormatNV16) ? 1 : 2;
        int8_t v_plane_index = (source.format == kSoftwareFormatNV16) ? 2 : 1;

        SoftwareFrameWithMemory i422 = ConvertManager::thread_temporary_frame(kSoftwareFormatI422, source.width, source.height);

        libyuv::SplitUVPlane(source.data[1], source.linesize[1],
                i422.data[u_plane_index], i422.linesize[u_plane_index],
                i422.data[v_plane_index], i422.linesize[v_plane_index],
            source.width / 2, source.height);
        libyuv::CopyPlane(source.data[0], source.linesize[0],
                i422.data[0], i422.linesize[0],
            source.width, source.height);

        libyuv::I422ToI420(source.data[0], source.linesize[0],
                i422.data[1], i422.linesize[1],
                i422.data[2], i422.linesize[2],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                dest.data[2], dest.linesize[2],
                source.width, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV16, kSoftwareFormatI420, nv16_nv61_to_i420);
    ConvertManager::add_converter(kSoftwareFormatNV61, kSoftwareFormatI420, nv16_nv61_to_i420);

    ConvertManager::add_converter(kSoftwareFormatYVYU422, kSoftwareFormatI420,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {

        libyuv::YUY2ToI420(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                dest.data[2], dest.linesize[2],
                dest.data[1], dest.linesize[1],
                source.width, source.height);
            return 0;
        });

    Converter nv24_nv42_to_i420 = [](
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
            source.width, source.height);

        // half uv width/height of 444 1W*1H -> 0.5W*0.5H
        libyuv::ScalePlane(i444.data[1], i444.linesize[1], i444.width, i444.height,
                dest.data[1], dest.linesize[1], source.width / 2, source.height / 2, libyuv::kFilterLinear);
        libyuv::ScalePlane(i444.data[2], i444.linesize[2], i444.width, i444.height,
                dest.data[2], dest.linesize[2], source.width / 2, source.height / 2, libyuv::kFilterLinear);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV24, kSoftwareFormatI420, nv24_nv42_to_i420);
    ConvertManager::add_converter(kSoftwareFormatNV42, kSoftwareFormatI420, nv24_nv42_to_i420);

    ConvertManager::add_converter(kSoftwareFormatYUV444, kSoftwareFormatI420,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
            SoftwareFrameWithMemory i444 = ConvertManager::thread_temporary_frame(kSoftwareFormatI444, source.width, source.height );

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
                    dest.data[1], dest.linesize[1], dest.width / 2, dest.height / 2, libyuv::kFilterLinear);
            libyuv::ScalePlane(i444.data[2], i444.linesize[2], i444.width, i444.height,
                    dest.data[2], dest.linesize[2], dest.width / 2, dest.height / 2, libyuv::kFilterLinear);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatGRAY8, kSoftwareFormatI420,
        [](const SoftwareFrame& source,SoftwareFrame& dest) -> int32_t {
        libyuv::CopyPlane(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                source.width, source.height);
        libyuv::SetPlane(dest.data[1], dest.linesize[1], dest.width / 2, dest.height / 2, 128);
        libyuv::SetPlane(dest.data[2], dest.linesize[2], dest.width / 2, dest.height / 2, 128);
            return 0;
        });
    ConvertManager::add_converter(kSoftwareFormatGRAY8A, kSoftwareFormatI420,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
            SoftwareFrameWithMemory alpha = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width, source.height);

            libyuv::SplitUVPlane(source.data[0], source.linesize[0],
                    dest.data[0], dest.linesize[0],
                    alpha.data[0], alpha.linesize[0],
                source.width, source.height);

            libyuv::SetPlane(dest.data[1], dest.linesize[1], dest.width / 2, dest.height / 2, 128);
            libyuv::SetPlane(dest.data[2], dest.linesize[2], dest.width / 2, dest.height / 2, 128);
            return 0;
        });
}
