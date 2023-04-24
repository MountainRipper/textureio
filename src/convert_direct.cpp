#include "convert_direct.h"
#include "convert_manager.h"

void ConvertDirect::register_direct_converter()
{
    swap_nvxx_uv_order();
    yuv_planer_to_gray();
    yuv_packeted_to_gray();
    //MARK:NV12ToNV24 is slower than nv12->i420->nv24
    //nvxx_convert_uv();

    rgb24_to_others();
}

void ConvertDirect::register_copy_converter()
{
    Converter copy_frame = [](const SoftwareFrame& source,
                                  SoftwareFrame& dest) -> int32_t {

        if(source.format != dest.format || source.width != dest.width || source.height != dest.height)
            return kErrorFormatNotMatch;
        if(source.width == 0 || source.height == 0 || dest.width == 0 || dest.height == 0)
            return kErrorInvalidFrame;

        uint8_t depth = 8;

        auto& planes = g_software_format_info[source.format].planes;
        for(int index = 0; index < 4; index++){
            auto& plane = planes[index];
            if(plane.channels == 0)
                break;

            int32_t plane_width = (source.width * plane.scale_x) * plane.channels * depth / 8;
            int32_t plane_height = source.height * plane.scale_y;
            if(source.data[index] == nullptr || source.data[index] == nullptr || dest.linesize[index] < plane_width)
                continue;

            libyuv::CopyPlane(source.data[index], source.linesize[index],
                              dest.data[index], dest.linesize[index],
                              plane_width,plane_height);
        }
        return 0;
    };
    for (int format_index = kSoftwareFormatFirst; format_index < kSoftwareFormatCount; format_index++) {
        SoftwareFrameFormat format = static_cast<SoftwareFrameFormat>(format_index);
        ConvertManager::add_converter(format, format, copy_frame);
    }
}


void ConvertDirect::swap_nvxx_uv_order()
{
    Converter swap_nvxx_uv = [](const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {

        auto& plane = g_software_format_info[source.format].planes[1];

        libyuv::CopyPlane(source.data[0],source.linesize[0],
                dest.data[0],dest.linesize[0],
                source.width,source.height);

        libyuv::SwapUVPlane(source.data[1],source.linesize[1],
                dest.data[1],dest.linesize[1],
                source.width * plane.scale_x,source.height * plane.scale_y);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV12, kSoftwareFormatNV21,swap_nvxx_uv);
    ConvertManager::add_converter(kSoftwareFormatNV21, kSoftwareFormatNV12,swap_nvxx_uv);
    ConvertManager::add_converter(kSoftwareFormatNV16, kSoftwareFormatNV61,swap_nvxx_uv);
    ConvertManager::add_converter(kSoftwareFormatNV61, kSoftwareFormatNV16,swap_nvxx_uv);
    ConvertManager::add_converter(kSoftwareFormatNV24, kSoftwareFormatNV42,swap_nvxx_uv);
    ConvertManager::add_converter(kSoftwareFormatNV42, kSoftwareFormatNV24,swap_nvxx_uv);
}

void ConvertDirect::yuv_planer_to_gray()
{
    Converter planer_yuv_to_gray = [](const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {

        libyuv::CopyPlane(source.data[0],source.linesize[0],
                dest.data[0],dest.linesize[0],
                source.width,source.height);

        return 0;
    };
    Converter planer_yuv_to_gray_alpha = [](const SoftwareFrame& source,
                                      SoftwareFrame& dest) -> int32_t {

        SoftwareFrameWithMemory alpha = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width, source.height);
        libyuv::SetPlane(alpha.data[0], alpha.linesize[0], source.width, source.height, 255);

        libyuv::MergeUVPlane(source.data[0], source.linesize[0],
                alpha.data[0], alpha.linesize[0],
                dest.data[0], dest.linesize[0],
            source.width, source.height);

        return 0;
    };

    for (int format_index = kSoftwareFormatYUVStart; format_index <= kSoftwareFormatYUVEnd; format_index++) {
        SoftwareFrameFormat format = static_cast<SoftwareFrameFormat>(format_index);
        if(format == kSoftwareFormatYUYV422 || format == kSoftwareFormatYVYU422 || format == kSoftwareFormatUYVY422 || format == kSoftwareFormatYUV444)
            continue;
        ConvertManager::add_converter(format, mr::tio::kSoftwareFormatGRAY8, planer_yuv_to_gray);
        ConvertManager::add_converter(format, mr::tio::kSoftwareFormatGRAY8A, planer_yuv_to_gray_alpha);
    }
}

void ConvertDirect::yuv_packeted_to_gray()
{
    Converter yuy2_to_gray = [](const SoftwareFrame& source,
                                 SoftwareFrame& dest) -> int32_t {
        SoftwareFrameWithMemory temp = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width, source.height );
        libyuv::YUY2ToI422(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                temp.data[0], temp.linesize[0],
                temp.data[0], temp.linesize[0],
            source.width, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatYUYV422, kSoftwareFormatGRAY8,yuy2_to_gray);
    ConvertManager::add_converter(kSoftwareFormatYVYU422, kSoftwareFormatGRAY8,yuy2_to_gray);
    ConvertManager::add_converter(kSoftwareFormatUYVY422, kSoftwareFormatGRAY8,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
            SoftwareFrameWithMemory temp = ConvertManager::thread_temporary_frame( kSoftwareFormatGRAY8, source.width, source.height);
            libyuv::UYVYToI422(source.data[0], source.linesize[0],
                    dest.data[0], dest.linesize[0],
                    temp.data[0], temp.linesize[0],
                    temp.data[0], temp.linesize[0],
                    source.width, source.height);
            return 0;
    });
    ConvertManager::add_converter(kSoftwareFormatYUV444, kSoftwareFormatGRAY8,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
            SoftwareFrameWithMemory temp = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width, source.height);
            libyuv::SplitRGBPlane(source.data[0], source.linesize[0],
                    dest.data[0], dest.linesize[0],
                    temp.data[0], temp.linesize[0],
                    temp.data[0], temp.linesize[0],
                source.width, source.height);

            return 0;
    });
}

void ConvertDirect::nvxx_convert_uv()
{
    Converter nv420_to_nv444 = [](const SoftwareFrame& source,
                                 SoftwareFrame& dest) -> int32_t {

        libyuv::NV12ToNV24(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
            source.width, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV12, kSoftwareFormatNV24,nv420_to_nv444);
    ConvertManager::add_converter(kSoftwareFormatNV21, kSoftwareFormatNV42,nv420_to_nv444);

    Converter nv420_to_nv444_swap = [](const SoftwareFrame& source,
                                 SoftwareFrame& dest) -> int32_t {

        SoftwareFrameWithMemory uv = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8A, source.width, source.height);

        libyuv::NV12ToNV24(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                dest.data[0], dest.linesize[0],
                uv.data[0], uv.linesize[0],
            source.width, source.height);
        libyuv::SwapUVPlane(uv.data[0], uv.linesize[0],
                dest.data[1], dest.linesize[1],
                source.width, source.height);
        return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV12, kSoftwareFormatNV42,nv420_to_nv444_swap);
    ConvertManager::add_converter(kSoftwareFormatNV21, kSoftwareFormatNV24,nv420_to_nv444_swap);

    Converter nv422_to_nv444 = [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {
        libyuv::NV16ToNV24(source.data[0], source.linesize[0],
                source.data[1], source.linesize[1],
                dest.data[0], dest.linesize[0],
                dest.data[1], dest.linesize[1],
                    source.width, source.height);
            return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV16, kSoftwareFormatNV24,nv422_to_nv444);
    ConvertManager::add_converter(kSoftwareFormatNV61, kSoftwareFormatNV42,nv422_to_nv444);

    Converter nv422_to_nv444_swap = [](const SoftwareFrame& source,SoftwareFrame& dest) -> int32_t {

            SoftwareFrameWithMemory uv = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8A, source.width, source.height);

            libyuv::NV16ToNV24(source.data[0], source.linesize[0],
                    source.data[1], source.linesize[1],
                    dest.data[0], dest.linesize[0],
                    uv.data[0], uv.linesize[0],
                    source.width, source.height);

            libyuv::SwapUVPlane(uv.data[0], uv.linesize[0],
                    dest.data[1], dest.linesize[1],
                    source.width, source.height);
            return 0;
    };
    ConvertManager::add_converter(kSoftwareFormatNV16, kSoftwareFormatNV42,nv422_to_nv444_swap);
    ConvertManager::add_converter(kSoftwareFormatNV61, kSoftwareFormatNV24,nv422_to_nv444_swap);
}

void ConvertDirect::rgb24_to_others()
{
    ConvertManager::add_converter(kSoftwareFormatRGB24, kSoftwareFormatBGR24,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {

        libyuv::RAWToRGB24(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                source.width, source.height);

            return 0;
    });
    ConvertManager::add_converter(kSoftwareFormatBGR24, kSoftwareFormatRGB24,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {

        libyuv::RGB24ToRAW(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                source.width, source.height);
            return 0;
    });

    ConvertManager::add_converter(kSoftwareFormatRGB24, mr::tio::kSoftwareFormatABGR32,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {

        libyuv::RAWToRGBA(source.data[0], source.linesize[0],
                dest.data[0], dest.linesize[0],
                source.width, source.height);
            return 0;
    });

    ConvertManager::add_converter(kSoftwareFormatRGB24, mr::tio::kSoftwareFormatGRAY8,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {

            SoftwareFrameWithMemory temp = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width/2, source.height/2);
            libyuv::RAWToI420(source.data[0], source.linesize[0],
                    dest.data[0], dest.linesize[0],
                    temp.data[0], temp.linesize[0],
                    temp.data[0], temp.linesize[0],
                source.width, source.height);
            return 0;
    });

    ConvertManager::add_converter(kSoftwareFormatBGR24, mr::tio::kSoftwareFormatGRAY8,
        [](const SoftwareFrame& source,
            SoftwareFrame& dest) -> int32_t {

            SoftwareFrameWithMemory temp = ConvertManager::thread_temporary_frame(kSoftwareFormatGRAY8, source.width/2, source.height/2);
            libyuv::RGB24ToI420(source.data[0], source.linesize[0],
                    dest.data[0], dest.linesize[0],
                    temp.data[0], temp.linesize[0],
                    temp.data[0], temp.linesize[0],
                source.width, source.height);
            return 0;
    });
}
