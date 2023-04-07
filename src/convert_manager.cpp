#include "convert_manager.h"
#include "convert_to_i420.h"
#include "convert_to_i422.h"
#include "convert_to_i444.h"
#include "convert_to_bgra.h"
std::map<uint64_t,Converter> ConvertManager::converters_;

void ConvertManager::init(){
    static bool initialized = false;
    if(initialized)
        return;
    initialized = true;

    ConvertToI420::register_converter();
    ConvertToI422::register_converter();
    ConvertToI444::register_converter();
    ConvertToBGRA::register_converter();
    /*
         * convert to i420 intermediate format
        */


    /*
         * convert to argb intermediate format
        */

    fprintf(stderr,"totle converters:%zu",converters_.size());

}

void ConvertManager::add_converter(SoftwareFrameFormat input, SoftwareFrameFormat output, Converter converter){
    uint64_t cookie = input*10000+output;
    converters_[cookie] = converter;
}

Converter ConvertManager::get_convertor(SoftwareFrameFormat input, SoftwareFrameFormat output){
    init();

    uint64_t cookie = input*10000+output;
    if(converters_.find(cookie) == converters_.end())
        return Converter();
    return converters_[cookie];
}
