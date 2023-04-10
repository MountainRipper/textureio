#ifndef CONVERTDIRECT_H
#define CONVERTDIRECT_H


class ConvertDirect
{
public:
    static void register_copy_converter();
    static void register_direct_converter();
private:
    static void swap_nvxx_uv_order();
    static void yuv_planer_to_gray();
    static void yuv_packeted_to_gray();

    static void nvxx_convert_uv();
    static void rgb24_to_others();
};

#endif // CONVERTDIRECT_H
