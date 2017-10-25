cmake_minimum_required(VERSION 3.5.1)
project(trajecmp)

set(CMAKE_CXX_STANDARD 14)

include(cmake/enable_cxx_compiler_flag_if_supported.cmake)
enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")
enable_cxx_compiler_flag_if_supported("-g")

include(.conan/conanbuildinfo.cmake)
conan_basic_setup()


add_executable(example example/example.cpp)
target_include_directories(example PUBLIC src)
target_link_libraries(example ${CONAN_LIBS})

add_executable(rx-example example/rx-example.cpp)
target_include_directories(rx-example PUBLIC src)
target_link_libraries(rx-example ${CONAN_LIBS})

add_executable(trajecmp-example example/trajecmp-example.cpp)
target_include_directories(trajecmp-example PUBLIC src)
target_link_libraries(trajecmp-example ${CONAN_LIBS})

add_executable(distance-info-example example/distance-info-example.cpp)
target_include_directories(distance-info-example PUBLIC src)
target_link_libraries(distance-info-example ${CONAN_LIBS})

add_subdirectory(example/sdl2)

set(TEST_FILES
        test/runner.cpp
        test/distance_test.cpp
        test/find_local_extrema_test.cpp
        test/range_test.cpp
        )

add_executable(catch ${TEST_FILES})
target_include_directories(catch PUBLIC src)
target_link_libraries(catch ${CONAN_LIBS})
