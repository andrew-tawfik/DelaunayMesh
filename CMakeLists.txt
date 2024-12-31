cmake_minimum_required(VERSION 3.5)

project(ShapeTriangulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ShapeTriangulation main.cpp
        include/point.h point.cpp
        include/triangle.h triangle.cpp
        include/mesh.h mesh.cpp
        include/json.hpp
)

# Add test executable
add_executable(mesh_test
        test/mesh_test.cpp
        include/point.h point.cpp
        include/triangle.h triangle.cpp
        include/mesh.h mesh.cpp
)

# Explicitly add GTest include directory and link library
target_include_directories(mesh_test PRIVATE /usr/local/Cellar/googletest/1.15.2/include)
target_link_directories(mesh_test PRIVATE /usr/local/Cellar/googletest/1.15.2/lib)
target_link_libraries(mesh_test PRIVATE gtest gtest_main pthread)


include(GNUInstallDirs)
install(TARGETS ShapeTriangulation
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
