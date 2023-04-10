#include "convert_manager.h"
#include "convert_to_i420.h"
#include "convert_to_i422.h"
#include "convert_to_i444.h"
#include "convert_to_bgra.h"
#include "convert_from_i420.h"
#include "convert_from_i422.h"
#include "convert_from_i444.h"
#include "convert_from_bgra.h"
#include "convert_direct.h"
#include <thread>

#define MAKE_CONVERTER_COOKIE(from,to) (from*10000+to)

std::map<uint32_t,Converter> ConvertManager::converters_;
std::set<uint32_t> ConvertManager::slower_converters_;
std::map<uint8_t,ThreadTemporaryFrame> ConvertManager::temporary_frames_;

void ConvertManager::init(){
    static bool initialized = false;
    if(initialized)
        return;
    initialized = true;

    ConvertDirect::register_copy_converter();

    ConvertToI420::register_converter();
    ConvertToI422::register_converter();
    ConvertToI444::register_converter();
    ConvertToBGRA::register_converter();

    ConvertFromI420::register_converter();
    ConvertFromI422::register_converter();
    ConvertFromI444::register_converter();
    ConvertFromBGRA::register_converter();

    ConvertDirect::register_direct_converter();
    fprintf(stderr,"totle converters:%zu\n",converters_.size());
}

void ConvertManager::add_converter(SoftwareFrameFormat input, SoftwareFrameFormat output, Converter converter){
    if( input >= kSoftwareFormatCount || output >= kSoftwareFormatCount)
        return;

    uint32_t cookie = MAKE_CONVERTER_COOKIE(input,output);
    if(converters_.find(cookie) != converters_.end()){
        fprintf(stderr,"Warnning: Converter from %s to %s already exsit, overwrite old.\n",
                g_soft_format_names[input],g_soft_format_names[output]);
    }
    converters_[cookie] = converter;
}

Converter ConvertManager::get_convertor(SoftwareFrameFormat input, SoftwareFrameFormat output){
    init();

    uint32_t cookie = MAKE_CONVERTER_COOKIE(input,output);
    if(converters_.find(cookie) == converters_.end())
        return Converter();
    return converters_[cookie];
}

void ConvertManager::mark_slower_convertor(SoftwareFrameFormat input, SoftwareFrameFormat output)
{
    uint32_t cookie = MAKE_CONVERTER_COOKIE(input,output);
    slower_converters_.insert(cookie);
}

bool ConvertManager::is_slower_convertor(SoftwareFrameFormat input, SoftwareFrameFormat output)
{
    uint32_t cookie = MAKE_CONVERTER_COOKIE(input,output);
    if(slower_converters_.find(cookie) != slower_converters_.end())
        return true;
    return false;
}

SoftwareFrameWithMemory ConvertManager::thread_temporary_frame(SoftwareFrameFormat format, uint32_t width, uint32_t height, uint8_t id)
{
    if(temporary_frames_.find(id) == temporary_frames_.end()){
        temporary_frames_[id] = ThreadTemporaryFrame();
    }
    return temporary_frames_[id].get_temporary_frame(width,height,format);
}
