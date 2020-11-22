cmake_minimum_required (VERSION 2.8)
project (SuperCollideEssentiaUGens)

include_directories(${SC_PATH}/include/plugin_interface)
include_directories(${SC_PATH}/include/common)
include_directories(${SC_PATH}/external_libraries/libsndfile/)

include_directories(${ESSENTIA_PATH}/lib)
include_directories(${ESSENTIA_PATH}/include/essentia)
include_directories(${EIGEN_PATH}/include/eigen3)

set(CMAKE_SHARED_MODULE_PREFIX "")
if(APPLE OR WIN32)
set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()

add_library(EssentiaHFC MODULE EssentiaHFC) 

file(GLOB LIBRARIES 
"${ESSENTIA_PATH}/lib/libessentia.dylib"
)
message("Dynamic linked libraries: ${LIBRARIES}")

target_link_libraries(EssentiaHFC ${LIBRARIES}) 

SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++0x -pthread")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})