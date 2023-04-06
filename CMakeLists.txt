cmake_minimum_required(VERSION 3.14)

project(textureio LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED ${MP_TARGET_OS})
    set(MP_TARGET_OS ${CMAKE_SYSTEM_NAME})
endif()
string(TOLOWER ${MP_TARGET_OS} MP_TARGET_OS)
if(NOT DEFINED  ${MP_TARGET_ARCH})
    set(MP_TARGET_ARCH ${CMAKE_SYSTEM_PROCESSOR})
endif()

if(${MP_TARGET_OS} MATCHES "linux")
    include_directories(/usr/include/drm)
elseif(${MP_TARGET_OS} MATCHES "windows")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(MP_TARGET_OS "mingw")
    endif()
elseif(${MP_TARGET_OS} MATCHES "darwin")

elseif(${MP_TARGET_OS} MATCHES "android")

elseif(${MP_TARGET_OS} MATCHES "ios")
endif()

if(${MP_TARGET_ARCH} MATCHES "AMD64")
    set(MP_TARGET_ARCH "x86_64")
endif()

set(THIRD_PARTY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../third_party")
set(THIRD_PARTY_TARGET "${THIRD_PARTY_ROOT}/targets/${MP_TARGET_OS}-${MP_TARGET_ARCH}")

message(STATUS "INFO:MAKE FOR OS:${MP_TARGET_OS} ARCH:${MP_TARGET_ARCH}")
include_directories(textureio PRIVATE
    "${THIRD_PARTY_ROOT}/include"
    "${THIRD_PARTY_ROOT}/include/imgui"
    "${THIRD_PARTY_TARGET}/include")
link_directories("${THIRD_PARTY_TARGET}/lib")


file( GLOB HEADERS include/tio/*.h)
file( GLOB SOURCES src/*.cpp src/*.h)
add_library(textureio STATIC
  ${HEADERS}
  ${SOURCES}
)
target_include_directories(textureio
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)


#target_link_libraries(video2hw PRIVATE )
#target_compile_definitions(video2hw PRIVATE)

add_subdirectory(test)
