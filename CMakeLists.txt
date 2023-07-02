cmake_minimum_required(VERSION 3.14)

project(textureio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#######################arch/os/3rd_party detect#######################
if(NOT DEFINED MR_TARGET_OS)
    set(MR_TARGET_OS ${CMAKE_SYSTEM_NAME})
endif()
string(TOLOWER ${MR_TARGET_OS} MR_TARGET_OS)
if(NOT DEFINED  MR_TARGET_ARCH)
    set(MR_TARGET_ARCH ${CMAKE_SYSTEM_PROCESSOR})
endif()


if(${MR_TARGET_OS} MATCHES "linux")
    include_directories(/usr/include/drm)
elseif(${MR_TARGET_OS} MATCHES "windows")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(MR_TARGET_OS "mingw")
    endif()
elseif(${MR_TARGET_OS} MATCHES "darwin")

elseif(${MR_TARGET_OS} MATCHES "android")

elseif(${MR_TARGET_OS} MATCHES "ios")
endif()

if(${MR_TARGET_ARCH} MATCHES "AMD64")
    set(MR_TARGET_ARCH "x86_64")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # using GCC
    set(CMAKE_CXX_FLAGS "-fPIC" )
    set(CMAKE_C_FLAGS "-fPIC" )
endif()

if(NOT DEFINED MR_THIRD_PARTY_ROOT)
    set(MR_THIRD_PARTY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../third_party")
    set(MR_THIRD_PARTY_TARGET "${MR_THIRD_PARTY_ROOT}/targets/${MR_TARGET_OS}-${MR_TARGET_ARCH}")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
        "${MR_THIRD_PARTY_ROOT}/include"
        "${MR_THIRD_PARTY_ROOT}/include/imgui"
        "${MR_THIRD_PARTY_TARGET}/include"
        "${MR_THIRD_PARTY_TARGET}/include/luajit"
        "${MR_THIRD_PARTY_TARGET}/include/freetype2")
    link_directories("${MR_THIRD_PARTY_TARGET}/lib")
    SET(MR_SEARCH_LIB_PATHS "${MR_THIRD_PARTY_TARGET}/lib")
endif()

message(STATUS "INFO:SDMP MAKE FOR OS:${MR_TARGET_OS} ARCH:${MR_TARGET_ARCH} SYSROOT:${CMAKE_SYSROOT}")
message(STATUS "INFO:SDMP MAKE USE PLATFORM THIRD-PARTY LIBS:${MR_SEARCH_LIB_PATHS}")
#######################arch/os/3rd_party detect end#######################


file( GLOB HEADERS include/tio/*.h)
file( GLOB SOURCES src/*.cpp src/*.h)
add_library(textureio STATIC
  ${HEADERS}
  ${SOURCES}
)
target_include_directories(textureio
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(textureio
    PUBLIC
    yuv)

#target_link_libraries(video2hw PRIVATE )
#target_compile_definitions(video2hw PRIVATE)

add_subdirectory(test)
