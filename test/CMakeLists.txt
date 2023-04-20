
add_executable(sdl_tio example_textureio.h
example_textureio.cpp
example_3rd_bundle.cpp
test_pic.h
test_png.h)
target_compile_definitions(sdl_tio PRIVATE CMAKE_FILE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" USE_GLES=0)


if(NOT DEFINED ${MP_TARGET_OS})
    set(MP_TARGET_OS ${CMAKE_SYSTEM_NAME})
endif()
string(TOLOWER ${MP_TARGET_OS} MP_TARGET_OS)
message(${MP_TARGET_OS})
if(${MP_TARGET_OS} MATCHES "linux")
    target_link_libraries(sdl_tio  glad SDL2 X11 mr-common textureio GLESv2 EGL)
elseif(${MP_TARGET_OS} MATCHES "windows")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_link_libraries(sdl_tio glad SDL2.dll mr-common textureio)
    endif()
elseif(${MP_TARGET_OS} MATCHES "darwin")

elseif(${MP_TARGET_OS} MATCHES "android")

elseif(${MP_TARGET_OS} MATCHES "ios")
endif()


file( GLOB SOURCE_PNG_PACKAGE png_merge.cpp)
add_executable(png_merge ${SOURCE_PNG_PACKAGE})
