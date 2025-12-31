cmake_minimum_required(VERSION 4.2)

project(ShapeTriangulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

include_directories(include)

add_executable(ShapeTriangulation
        src/main.cpp
        src/point.cpp
        src/triangle.cpp
        src/mesh.cpp
)

# Link websocketpp dependencies (Boost and threads)
target_link_libraries(ShapeTriangulation PRIVATE 
    Boost::boost 
    Threads::Threads
)

find_package(GTest REQUIRED)

add_executable(mesh_test
        test/mesh_test.cpp
        src/point.cpp
        src/triangle.cpp
        src/mesh.cpp
)

add_executable(benchmark
        test/mesh_benchmark.cpp
        src/point.cpp
        src/triangle.cpp
        src/mesh.cpp
)

target_link_libraries(mesh_test PRIVATE GTest::gtest GTest::gtest_main pthread)


include(GNUInstallDirs)
install(TARGETS ShapeTriangulation
    BUNDLE DESTINATION ..
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

