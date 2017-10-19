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

set(SDL2_LIBRARIES "-L${SDL2_LIBDIR} -lSDL2") # fix https://stackoverflow.com/questions/45730098/cmake-leading-or-trailing-whitespace-policy-cmp0004#comment78421781_45730098
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2GFX_INCLUDE_DIR})

include_directories(include)

add_executable(sdl2-line-example src/sdl2-line-example.cpp)
target_include_directories(sdl2-line-example PUBLIC src)
target_link_libraries(sdl2-line-example ${CONAN_LIBS} ${SDL2_LIBRARIES} ${SDL2GFX_LIBRARY})
