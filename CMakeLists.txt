cmake_minimum_required(VERSION 3.14)

project(textureio LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(textureio STATIC
  textureio.cpp
  textureio.h
)

#target_link_libraries(video2hw PRIVATE )
#target_compile_definitions(video2hw PRIVATE)
