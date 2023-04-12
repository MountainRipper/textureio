
file( GLOB SOURCE_EXAMPLE *.cpp *.h)
add_executable(glfw_tio ${SOURCE_EXAMPLE} ${SOURCE_IMGUI})
target_compile_definitions(glfw_tio PRIVATE CMAKE_FILE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" USE_GLES=0)

if(NOT DEFINED ${MP_TARGET_OS})
    set(MP_TARGET_OS ${CMAKE_SYSTEM_NAME})
endif()
string(TOLOWER ${MP_TARGET_OS} MP_TARGET_OS)
message(${MP_TARGET_OS})
if(${MP_TARGET_OS} MATCHES "linux")
    target_link_libraries(glfw_tio  glad glfw X11 mr-common textureio GLESv2 EGL)
elseif(${MP_TARGET_OS} MATCHES "windows")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_link_libraries(glfw_tio glad glfw3dll mr-common textureio)
    endif()
elseif(${MP_TARGET_OS} MATCHES "darwin")

elseif(${MP_TARGET_OS} MATCHES "android")

elseif(${MP_TARGET_OS} MATCHES "ios")
endif()
