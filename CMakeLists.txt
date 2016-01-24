cmake_minimum_required(VERSION 2.8.9)
project(MagnumTriangleExample)

find_package(Magnum REQUIRED
    Shaders
    Sdl2Application
    SceneGraph
    Primitives
    MeshTools)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CORRADE_CXX_FLAGS}")
include_directories(${MAGNUM_INCLUDE_DIRS} ${MAGNUM_APPLICATION_INCLUDE_DIRS})

add_executable(magnum-triangle main.cpp)
target_link_libraries(magnum-triangle
    ${MAGNUM_APPLICATION_LIBRARIES}
    ${MAGNUM_LIBRARIES}
    ${MAGNUM_SHADERS_LIBRARIES}
    ${MAGNUM_SCENEGRAPH_LIBRARIES}
    ${MAGNUM_PRIMITIVES_LIBRARIES}
    ${MAGNUM_MESHTOOLS_LIBRARIES})

