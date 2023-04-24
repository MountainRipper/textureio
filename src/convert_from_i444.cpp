#include "convert_from_i444.h"
#include "convert_manager.h"
void ConvertFromI444::register_converter()
{

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatI420,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{

        libyuv::I444ToI420(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                source.data[2], source.linesize[2],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                dest.data[2], dest.linesize[2],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatYV12,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{
        libyuv::I444ToI420(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                source.data[2], source.linesize[2],
                dest.data[0], dest.linesize[0],
                dest.data[2], dest.linesize[2],
                dest.data[1], dest.linesize[1],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatNV12,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{
        libyuv::I444ToNV12(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                source.data[2], source.linesize[2],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatNV21,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{

        libyuv::I444ToNV21(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                source.data[2], source.linesize[2],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                source.width, source.height);
        return  0;
    });


    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatI422,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{

        libyuv::CopyPlane(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                source.width, source.height);

        // half uv width of 444 1W*1H -> 0.5W*1H
        libyuv::ScalePlane(source.data[1], source.linesize[1], source.width, source.height,
                dest.data[1], dest.linesize[1], source.width / 2, source.height, libyuv::kFilterLinear);
        libyuv::ScalePlane(source.data[2], source.linesize[2], source.width, source.height,
                dest.data[2], dest.linesize[2], source.width / 2, source.height, libyuv::kFilterLinear);

        return  0;
    });

    Converter i444_to_nv16_nv61 = [](const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {
        int8_t u_plane_index = (dest.format == kSoftwareFormatNV16) ? 1 : 2;
        int8_t v_plane_index = (dest.format == kSoftwareFormatNV16) ? 2 : 1;

        libyuv::CopyPlane(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                source.width, source.height);

        SoftwareFrameWithMemory i422 = ConvertManager::thread_temporary_frame(kSoftwareFormatI422, source.width, source.height);


        // half uv width of 444 1W*1H -> 0.5W*1H
        libyuv::ScalePlane(source.data[1], source.linesize[1], source.width, source.height,
                i422.data[1], i422.linesize[1], dest.width / 2, dest.height, libyuv::kFilterLinear);
        libyuv::ScalePlane(source.data[2], source.linesize[2], source.width, source.height,
                i422.data[2], i422.linesize[2], dest.width / 2, dest.height, libyuv::kFilterLinear);

        libyuv::MergeUVPlane(i422.data[u_plane_index], i422.linesize[u_plane_index],
                             i422.data[v_plane_index], i422.linesize[v_plane_index],
                             dest.data[1], dest.linesize[1],
            source.width / 2, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatI444, kSoftwareFormatNV16, i444_to_nv16_nv61);
    ConvertManager::add_converter(kSoftwareFormatI444, kSoftwareFormatNV61, i444_to_nv16_nv61);

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatYUYV422,
                                  [](const SoftwareFrame& source,SoftwareFrame& dest)->int32_t{

        SoftwareFrameWithMemory i422 = ConvertManager::thread_temporary_frame(kSoftwareFormatI422, source.width, source.height);

        auto i444_to_i422 = ConvertManager::get_convertor(kSoftwareFormatI444,kSoftwareFormatI422);
        i444_to_i422(source,i422);

        libyuv::I422ToYUY2(i422.data[0], i422.linesize[0],
                i422.data[1], i422.linesize[1],
                i422.data[2], i422.linesize[2],
                dest.data[0], dest.linesize[0],
                source.width, source.height);

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatYVYU422,
                  [](const SoftwareFrame& source,SoftwareFrame& dest)->int32_t{

        SoftwareFrameWithMemory i422 = ConvertManager::thread_temporary_frame(kSoftwareFormatI422, source.width, source.height);

        auto i444_to_i422 = ConvertManager::get_convertor(kSoftwareFormatI444,kSoftwareFormatI422);
        i444_to_i422(source,i422);

        libyuv::I422ToYUY2(i422.data[0], i422.linesize[0],
                i422.data[2], i422.linesize[2],
                i422.data[1], i422.linesize[1],
                dest.data[0], dest.linesize[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatUYVY422,
                  [](const SoftwareFrame& source,SoftwareFrame& dest)->int32_t{

        SoftwareFrameWithMemory i422 = ConvertManager::thread_temporary_frame(kSoftwareFormatI422, source.width, source.height);

        auto i444_to_i422 = ConvertManager::get_convertor(kSoftwareFormatI444,kSoftwareFormatI422);
        i444_to_i422(source,i422);

        libyuv::I422ToUYVY(i422.data[0], i422.linesize[0],
                i422.data[1], i422.linesize[1],
                i422.data[2], i422.linesize[2],
                dest.data[0], dest.linesize[0],
                source.width, source.height);

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatI444,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{

        libyuv::I444Copy(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                source.data[2], source.linesize[2],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                dest.data[2], dest.linesize[2],
                source.width, source.height);

        return  0;
    });

    Converter i444_to_nv24_nv42 = [](const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {
        int8_t u_plane_index = (dest.format == kSoftwareFormatNV24) ? 1 : 2;
        int8_t v_plane_index = (dest.format == kSoftwareFormatNV24) ? 2 : 1;

        SoftwareFrameWithMemory i444 = ConvertManager::thread_temporary_frame(kSoftwareFormatI444, source.width, source.height);


        libyuv::CopyPlane(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
            source.width, source.height);

        libyuv::MergeUVPlane(source.data[u_plane_index], source.linesize[u_plane_index],
                             source.data[v_plane_index], source.linesize[v_plane_index],
                             dest.data[1], dest.linesize[1],
            source.width, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatI444, kSoftwareFormatNV24, i444_to_nv24_nv42);
    ConvertManager::add_converter(kSoftwareFormatI444, kSoftwareFormatNV42, i444_to_nv24_nv42);

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatYUV444,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{

        libyuv::MergeRGBPlane(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                source.data[2], source.linesize[2],
                dest.data[0], dest.linesize[0],
            source.width, source.height);

        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatRGB24,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{

        libyuv::I444ToRAW(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                source.data[2], source.linesize[2],
                dest.data[0], dest.linesize[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatBGR24,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{

        libyuv::I444ToRGB24(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                source.data[2], source.linesize[2],
                dest.data[0], dest.linesize[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatRGBA32,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{

        libyuv::I444ToABGR(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                source.data[2], source.linesize[2],
                dest.data[0], dest.linesize[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatBGRA32,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{

        libyuv::I444ToARGB(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                source.data[2], source.linesize[2],
                dest.data[0], dest.linesize[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatARGB32,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{
        SoftwareFrameWithMemory bgra = ConvertManager::thread_temporary_frame(kSoftwareFormatBGRA32, source.width, source.height);

        libyuv::I444ToARGB(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                source.data[2], source.linesize[2],
                bgra.data[0], bgra.linesize[0],
                source.width, source.height);
        libyuv::ARGBToBGRA(bgra.data[0], bgra.linesize[0],
                dest.data[0], dest.linesize[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatABGR32,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{

        SoftwareFrameWithMemory bgra = ConvertManager::thread_temporary_frame(kSoftwareFormatBGRA32, source.width, source.height);

        libyuv::I444ToARGB(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                source.data[2], source.linesize[2],
                bgra.data[0], bgra.linesize[0],
                source.width, source.height);
        libyuv::ARGBToRGBA(bgra.data[0], bgra.linesize[0],
                dest.data[0], dest.linesize[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatGRAY8,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{

        libyuv::CopyPlane(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                source.width, source.height);
        return  0;
    });

    ConvertManager::add_converter(kSoftwareFormatI444,kSoftwareFormatGRAY8A,
                  [](const SoftwareFrame& source,
                  SoftwareFrame& dest)->int32_t{
        SoftwareFrameWithMemory alpha = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width, source.height);

        libyuv::SetPlane(alpha.data[0], alpha.linesize[0], source.width, source.height, 255);

        libyuv::MergeUVPlane(source.data[0], source.linesize[0],
                alpha.data[0], alpha.linesize[0],
                dest.data[0], dest.linesize[0],
            source.width, source.height);
        return  0;
    });

}
