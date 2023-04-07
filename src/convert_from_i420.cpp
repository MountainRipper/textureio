#include "convert_from_i420.h"
#include "convert_manager.h"

// using namespace libyuv;

void ConvertFromI420::register_converter()
{
    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatI420,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I420Copy(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                dest.data[2], dest.line_size[2],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatYV12,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I420Copy(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                dest.data[2], dest.line_size[2],
                dest.data[1], dest.line_size[1],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatNV12,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I420ToNV12(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                source.width, source.height);

            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatNV21,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I420ToNV21(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatI422,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I420ToI422(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                dest.data[1], dest.line_size[1],
                dest.data[2], dest.line_size[2],
                source.width, source.height);
            return 0;
        });

    Converter i420_to_nv16_nv61 = [](SoftwareFrameWithMemory& source,
                                      SoftwareFrameWithMemory& dest,
                                      RotationMode rotate,
                                      const CropArea& crop_area) -> int32_t {
        int8_t u_plane_index = (dest.format == kSoftwareFormatNV16) ? 1 : 2;
        int8_t v_plane_index = (dest.format == kSoftwareFormatNV16) ? 2 : 1;

        SoftwareFrameWithMemory i422 = { { kSoftwareFormatI422, source.width, source.height } };
        i422.alloc();

        libyuv::I420ToI422(source.data[0], source.line_size[0],
            source.data[1], source.line_size[1],
            source.data[2], source.line_size[2],
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
    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatNV16, i420_to_nv16_nv61);
    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatNV61, i420_to_nv16_nv61);

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatYUYV422,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I420ToYUY2(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);

            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatYVYU422,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            SoftwareFrameWithMemory yuyv = { { kSoftwareFormatYUYV422, source.width, source.height } };
            yuyv.alloc();

            libyuv::I420ToYUY2(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                yuyv.data[0], yuyv.line_size[0],
                source.width, source.height);

            libyuv::ARGBToABGR(yuyv.data[0], yuyv.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width / 2, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatUYVY422,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I420ToUYVY(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);

            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatI444,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            SoftwareFrameWithMemory i444 = { { kSoftwareFormatI444, source.width, source.height } };
            i444.alloc();
            libyuv::I420ToI444(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                i444.data[1], i444.line_size[1],
                i444.data[2], i444.line_size[2],
                source.width, source.height);

            return 0;
        });

    Converter i420_to_nv24_nv42 = [](SoftwareFrameWithMemory& source,
                                      SoftwareFrameWithMemory& dest,
                                      RotationMode rotate,
                                      const CropArea& crop_area) -> int32_t {
        int8_t u_plane_index = (dest.format == kSoftwareFormatNV24) ? 1 : 2;
        int8_t v_plane_index = (dest.format == kSoftwareFormatNV24) ? 2 : 1;

        SoftwareFrameWithMemory i444 = { { kSoftwareFormatI444, source.width, source.height } };
        i444.alloc();

        libyuv::I420ToI444(source.data[0], source.line_size[0],
            source.data[1], source.line_size[1],
            source.data[2], source.line_size[2],
            dest.data[0], dest.line_size[0],
            i444.data[u_plane_index], i444.line_size[u_plane_index],
            i444.data[v_plane_index], i444.line_size[v_plane_index],
            source.width, source.height);

        libyuv::MergeUVPlane(i444.data[1], i444.line_size[1],
            i444.data[2], i444.line_size[2],
            dest.data[1], dest.line_size[1],
            source.width / 2, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatNV24, i420_to_nv24_nv42);
    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatNV42, i420_to_nv24_nv42);

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatYUV444,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            SoftwareFrameWithMemory i444 = { { kSoftwareFormatI444, source.width, source.height } };
            i444.alloc();

            libyuv::I420ToI444(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                i444.data[0], i444.line_size[0],
                i444.data[1], i444.line_size[1],
                i444.data[2], i444.line_size[2],
                source.width, source.height);

            libyuv::MergeRGBPlane(i444.data[0], i444.line_size[0],
                i444.data[1], i444.line_size[1],
                i444.data[2], i444.line_size[2],
                dest.data[0], i444.line_size[0],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatRGB24,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I420ToRAW(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatBGR24,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I420ToRGB24(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatRGBA32,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I420ToABGR(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatBGRA32,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I420ToARGB(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatARGB32,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I420ToBGRA(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatABGR32,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::I420ToRGBA(source.data[0], source.line_size[0],
                source.data[1], source.line_size[1],
                source.data[2], source.line_size[2],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatGRAY8,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            libyuv::CopyPlane(source.data[0], source.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width, source.height);
            return 0;
        });

    ConvertManager::add_converter(kSoftwareFormatI420, kSoftwareFormatGRAY8A,
        [](SoftwareFrameWithMemory& source,
            SoftwareFrameWithMemory& dest,
            RotationMode rotate,
            const CropArea& crop_area) -> int32_t {
            SoftwareFrameWithMemory alpha = { { kSoftwareFormatGRAY8, source.width, source.height } };
            alpha.alloc();
            libyuv::SetPlane(alpha.data[0], alpha.line_size[0], source.width, source.height, 255);

            libyuv::MergeUVPlane(source.data[0], source.line_size[0],
                alpha.data[0], alpha.line_size[0],
                dest.data[0], dest.line_size[0],
                source.width, source.height);

            return 0;
        });
}
