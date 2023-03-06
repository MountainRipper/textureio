
file( GLOB SOURCE_EXAMPLE *.cpp *.h)
add_executable(glfw_tio ${SOURCE_EXAMPLE} ${SOURCE_IMGUI})
target_compile_definitions(glfw_tio PRIVATE CMAKE_FILE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" USE_GLES=0)
target_link_libraries(glfw_tio  glad glfw X11 mp-common textureio GLESv2 EGL)

