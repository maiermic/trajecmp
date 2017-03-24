cmake_minimum_required(VERSION 3.5.1)
project(trajectory-detection)

set(CMAKE_CXX_STANDARD 14)

include(cmake/enable_cxx_compiler_flag_if_supported.cmake)
enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")
enable_cxx_compiler_flag_if_supported("-g")

include(.conan/conanbuildinfo.cmake)
conan_basic_setup()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SOURCE_FILES
        src/Distance.cpp src/Distance.h
        src/TrajectorySvg.h
        src/DistinctColorSet.cpp src/DistinctColorSet.h
        src/distance.h)
set(CATCH_TEST_FILES
        test/runner.cpp
        test/boost_geometry_to_string.h
        test/logging.h
        test/distance_test.cpp
        test/CatchTest.cpp test/logging.h)

add_executable(boost_geometry_equals_bug src/boost_geometry_equals_bug.cpp ${SOURCE_FILES})
target_link_libraries(boost_geometry_equals_bug ${CONAN_LIBS})

add_executable(generate_svg src/generate_svg.cpp ${SOURCE_FILES})
target_link_libraries(generate_svg ${CONAN_LIBS})

add_executable(catch ${CATCH_TEST_FILES} ${SOURCE_FILES})
target_include_directories(catch PUBLIC .)
target_link_libraries(catch ${CONAN_LIBS} Threads::Threads)
