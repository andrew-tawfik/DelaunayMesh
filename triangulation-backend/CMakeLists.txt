cmake_minimum_required(VERSION 3.5)

project(ShapeTriangulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_definitions(-DASIO_STANDALONE)

find_package(websocketpp REQUIRED)

if(websocketpp_FOUND)
    message(STATUS "WebSocket++ found: ${websocketpp_INCLUDE_DIR}")
    include_directories(${websocketpp_INCLUDE_DIR})
else()
    message(FATAL_ERROR "WebSocket++ not found!")
endif()


# Include Boost headers and libraries
find_package(Boost REQUIRED COMPONENTS system thread) # Add other Boost components if needed

if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost not found!")
endif()


add_executable(ShapeTriangulation
        src/main.cpp
        src/point.cpp
        src/triangle.cpp
        src/mesh.cpp
)

target_link_libraries(ShapeTriangulation Boost::boost Boost::system Boost::thread)

include(GNUInstallDirs)
install(TARGETS ShapeTriangulation
    BUNDLE DESTINATION ..
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
