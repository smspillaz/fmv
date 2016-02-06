cmake_minimum_required(VERSION 2.8.9)
project(MagnumTriangleExample)

find_package(Magnum REQUIRED
    Shaders
    Sdl2Application
    SceneGraph
    Primitives
    MeshTools)

include(FindPkgConfig)

pkg_check_modules(LIBVISUAL REQUIRED libvisual-0.5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CORRADE_CXX_FLAGS}")
include_directories(SYSTEM
                    ${MAGNUM_INCLUDE_DIRS}
                    ${MAGNUM_APPLICATION_INCLUDE_DIRS}
                    ${LIBVISUAL_INCLUDE_DIRS})

add_library (fmv-view STATIC view.cpp)
target_link_libraries(fmv-view
             ${MAGNUM_APPLICATION_LIBRARIES}
             ${MAGNUM_LIBRARIES}
             ${MAGNUM_SHADERS_LIBRARIES}
             ${MAGNUM_SCENEGRAPH_LIBRARIES}
             ${MAGNUM_PRIMITIVES_LIBRARIES}
             ${MAGNUM_MESHTOOLS_LIBRARIES})

add_library (fmv-frequencyprovider STATIC frequencyprovider.cpp)

add_library (fmv-lv STATIC lvfrequencyprovider.cpp)
target_link_libraries(fmv-lv
             ${LIBVISUAL_LIBRARIES}
             fmv-frequencyprovider)

add_executable(fmv main.cpp)
target_link_libraries(fmv fmv-view fmv-lv)
