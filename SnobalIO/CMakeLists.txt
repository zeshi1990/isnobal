cmake_minimum_required(VERSION 3.6)
project(SnobalIO)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
INCLUDE_DIRECTORIES(/usr/local/ipw/)
LINK_DIRECTORIES(/usr/local/ipw/lib/)

set(SOURCE_FILES library.c library.h)
add_library(SnobalIO SHARED ${SOURCE_FILES})
set_target_properties(SnobalIO PROPERTIES SUFFIX ".so")
TARGET_LINK_LIBRARIES(SnobalIO libipw.a libgctp.a libimage.a libmodel.a libsnobal.a libstropts.a libtopo.a)