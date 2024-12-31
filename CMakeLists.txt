cmake_minimum_required(VERSION 3.5)

project(ShapeTriangulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(ShapeTriangulation src/main.cpp
        src/point.cpp
        src/triangle.cpp
        src/mesh.cpp
)

# Add test executable
add_executable(mesh_test
        test/mesh_test.cpp
        src/point.cpp
        src/triangle.cpp
        src/mesh.cpp
)

include_directories(/opt/restbed/source)
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
