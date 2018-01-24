cmake_minimum_required(VERSION 3.5.1)
project(trajecmp)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/")

include(../../cmake/enable_cxx_compiler_flag_if_supported.cmake)
enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")
enable_cxx_compiler_flag_if_supported("-g")

include(../../.conan/conanbuildinfo.cmake)
conan_basic_setup()

find_package(SDL2 REQUIRED)
find_package(SDL2GFX REQUIRED)
find_package(SDL2_ttf REQUIRED)

set(SDL2_LIBRARIES "-L${SDL2_LIBDIR} -lSDL2") # fix https://stackoverflow.com/questions/45730098/cmake-leading-or-trailing-whitespace-policy-cmp0004#comment78421781_45730098
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2GFX_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})

include_directories(include)

set(SRC_FILES
        src/sdl2_framework.cpp
        src/record_trajectory_sdl2_framework.cpp
        src/draw.cpp
        src/model.cpp
        src/notification_box.cpp
        src/text_renderer.cpp
)

function(add_app name)
    add_executable(${name} app/${name}.cpp ${SRC_FILES})
    target_include_directories(${name} PUBLIC src)
    target_include_directories(${name} PUBLIC ../../src)
    target_link_libraries(${name} ${CONAN_LIBS} ${SDL2_LIBRARIES} ${SDL2GFX_LIBRARY} ${SDL2_TTF_LIBRARY})
endfunction(add_app)

add_app(sdl2-example)
add_app(sdl2-circle-or-rectangle)
add_app(sdl2-count-circles)
add_app(sdl2-estimate-center-of-circle-segment)
add_app(sdl2-rectangle)
add_app(sdl2-rectangle-in-circle)
add_app(sdl2-triangle)
add_app(sdl2-ttf-example)
