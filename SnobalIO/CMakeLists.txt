cmake_minimum_required(VERSION 3.6)
project(SnobalIO)

set(CMAKE_C_COMPILER /usr/local/bin/gcc)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
INCLUDE_DIRECTORIES(/usr/local/ipw/)
LINK_DIRECTORIES(/usr/local/ipw/lib/)

file(GLOB SnobalIO_SRC
        "*.h"
        "*.c")
set(SOURCE_FILES ${SnobalIO_SRC})
add_library(SnobalIO SHARED ${SOURCE_FILES})
set_target_properties(SnobalIO PROPERTIES SUFFIX ".so")
TARGET_LINK_LIBRARIES(SnobalIO ipw gctp image model stropts topo snobal)