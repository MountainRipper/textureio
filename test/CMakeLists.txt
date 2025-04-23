
add_executable(sdl_tio example_textureio.h
example_textureio.cpp
test_pic.h
test_png.h)
target_compile_definitions(sdl_tio PRIVATE CMAKE_FILE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" USE_GLES=0)


if(NOT DEFINED ${MR_TARGET_OS})
    set(MR_TARGET_OS ${CMAKE_SYSTEM_NAME})
endif()
string(TOLOWER ${MR_TARGET_OS} MR_TARGET_OS)
message(${MR_TARGET_OS})
if(${MR_TARGET_OS} MATCHES "linux")
    target_link_libraries(sdl_tio  spdlog textureio GLESv2 EGL SDL3 X11 )
elseif(${MR_TARGET_OS} MATCHES "windows")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_link_libraries(sdl_tio spdlog textureio dwmapi  SDL3.dll )
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_link_libraries(sdl_tio spdlog textureio dwmapi  SDL3.dll )
    endif()
elseif(${MR_TARGET_OS} MATCHES "darwin")
    target_link_libraries(sdl_tio  spdlog textureio SDL3)
elseif(${MR_TARGET_OS} MATCHES "android")

elseif(${MR_TARGET_OS} MATCHES "ios")
endif()


file( GLOB SOURCE_PNG_PACKAGE png_merge.cpp)
add_executable(png_merge ${SOURCE_PNG_PACKAGE})
target_link_libraries(png_merge textureio)
