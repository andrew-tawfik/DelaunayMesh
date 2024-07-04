cmake_minimum_required(VERSION 3.5)

project(ShapeTriangulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ShapeTriangulation main.cpp
    point.h point.cpp
    triangle.h triangle.cpp
    mesh.h mesh.cpp
    README.md)

include(GNUInstallDirs)
install(TARGETS ShapeTriangulation
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
