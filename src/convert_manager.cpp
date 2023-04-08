#include "convert_manager.h"
#include "convert_to_i420.h"
#include "convert_to_i422.h"
#include "convert_to_i444.h"
#include "convert_to_bgra.h"
#include "convert_from_i420.h"
#include "convert_from_i422.h"
#include "convert_from_i444.h"
#include "convert_from_bgra.h"
std::map<uint32_t,Converter> ConvertManager::converters_;

void ConvertManager::init(){
    static bool initialized = false;
    if(initialized)
        return;
    initialized = true;

    ConvertToI420::register_converter();
    ConvertToI422::register_converter();
    ConvertToI444::register_converter();
    ConvertToBGRA::register_converter();

    fprintf(stderr,"totle converters:%zu\n",converters_.size());

    ConvertFromI420::register_converter();
    ConvertFromI422::register_converter();
    ConvertFromI444::register_converter();
    ConvertFromBGRA::register_converter();
    /*
         * convert to i420 intermediate format
        */


    /*
         * convert to argb intermediate format
        */

    fprintf(stderr,"totle converters:%zu\n",converters_.size());

}

void ConvertManager::add_converter(SoftwareFrameFormat input, SoftwareFrameFormat output, Converter converter){
    if( input >= kSoftwareFormatCount || output >= kSoftwareFormatCount)
        return;

    uint32_t cookie = input*10000+output;
    if(converters_.find(cookie) != converters_.end()){
        fprintf(stderr,"Warnning: Converter from %s to %s already exsit, overwrite old.\n",
                g_soft_format_names[input],g_soft_format_names[output]);
    }
    converters_[cookie] = converter;
}

Converter ConvertManager::get_convertor(SoftwareFrameFormat input, SoftwareFrameFormat output){
    init();

    uint32_t cookie = input*10000+output;
    if(converters_.find(cookie) == converters_.end())
        return Converter();
    return converters_[cookie];
}
