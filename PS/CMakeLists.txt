cmake_minimum_required(VERSION 2.8)
project(ORB_SLAM2)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

set( CMAKE_SYSTEM_NAME Linux )
set( CMAKE_SYSTEM_PROCESSOR arm )
set( CMAKE_C_COMPILER arm-linux-gnueabihf-gcc )
set( CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++ )
set( CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install )
set( PKG_CONFIG_LIBDIR ${PROJECT_SOURCE_DIR}/Thirdparty/lib/pkgconfig )
# set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
# set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
# set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=armv7-a")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall   -O3 -march=armv7-a")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-DCOMPILEDWITHC11)
message(STATUS "Using flag -std=c++11.")

include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include
${PROJECT_SOURCE_DIR}/Thirdparty/eigen3
${PROJECT_SOURCE_DIR}/Thirdparty/OpenCV
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(${PROJECT_NAME} SHARED
src/System.cc
src/Tracking.cc
src/LocalMapping.cc
src/LoopClosing.cc
src/ORBextractor.cc
src/ORBmatcher.cc
src/FrameDrawer.cc
src/Converter.cc
src/MapPoint.cc
src/KeyFrame.cc
src/Map.cc
# src/MapDrawer.cc
src/Optimizer.cc
src/PnPsolver.cc
src/Frame.cc
src/KeyFrameDatabase.cc
src/Sim3Solver.cc
src/Initializer.cc
# src/Viewer.cc
)

target_link_libraries(${PROJECT_NAME}
   ${PROJECT_SOURCE_DIR}/Thirdparty/DBoW2/lib/libDBoW2.so
   ${PROJECT_SOURCE_DIR}/Thirdparty/g2o/lib/libg2o.so
   ${PROJECT_SOURCE_DIR}/Thirdparty/OpenCV/lib/libopencv_dnn.so
)

# Build examples

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/RGB-D)

# add_executable(rgbd_tum
# Examples/RGB-D/rgbd_tum.cc)
# target_link_libraries(rgbd_tum ${PROJECT_NAME})
