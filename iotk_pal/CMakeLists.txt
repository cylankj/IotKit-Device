cmake_minimum_required(VERSION 2.8)
set(name iotk_pal)
project (${name} C)

include_directories(inc)

file (GLOB_RECURSE src src/*.c)
file (GLOB_RECURSE hdr inc/*.h)

add_library(${name}-lib STATIC ${src} ${hdr})

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/app)
	file (GLOB_RECURSE app app/*.c app/*.h)
	
    add_executable(${name}-test ${app})
    target_link_libraries(${name}-test ${name}-lib)	
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(${name}-test pthread)	
endif()
