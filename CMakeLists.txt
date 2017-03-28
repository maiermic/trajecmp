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
        src/TrajectorySvg.h
        src/DistinctColorSet.cpp src/DistinctColorSet.h
        src/distance.h
        src/Miniball.hpp
        src/coordinate_iterator.h
        src/logging.h
        src/boost_geometry_to_string.h
        )
set(CATCH_TEST_FILES
        test/runner.cpp
        test/distance_test.cpp
        )

add_executable(iterate_coordinates example/iterate_coordinates.cpp src/Miniball.hpp)
target_link_libraries(iterate_coordinates ${CONAN_LIBS})

add_executable(boost_optional example/boost_optional.cpp ${SOURCE_FILES} src/boost_geometry_to_string.h)
target_link_libraries(boost_optional ${CONAN_LIBS})

add_executable(boost_geometry_equals_bug example/boost_geometry_equals_bug.cpp ${SOURCE_FILES})
target_link_libraries(boost_geometry_equals_bug ${CONAN_LIBS})

add_executable(example example/example.cpp ${SOURCE_FILES})
target_link_libraries(example ${CONAN_LIBS})

add_executable(generate_svg example/generate_svg.cpp ${SOURCE_FILES})
target_link_libraries(generate_svg ${CONAN_LIBS})

add_executable(miniball_boost_geometry_example example/miniball_boost_geometry_example.cpp ${SOURCE_FILES})
target_link_libraries(miniball_boost_geometry_example ${CONAN_LIBS})

add_executable(miniball_example_containers example/miniball_example_containers.cpp ${SOURCE_FILES})
target_link_libraries(miniball_example_containers)

add_executable(catch ${CATCH_TEST_FILES} ${SOURCE_FILES})
target_include_directories(catch PUBLIC .)
target_link_libraries(catch ${CONAN_LIBS} Threads::Threads)
