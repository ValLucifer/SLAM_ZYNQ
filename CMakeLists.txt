cmake_minimum_required(VERSION 2.8)
project(ORB_SLAM2)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

set( CMAKE_SYSTEM_NAME Linux )
set( CMAKE_SYSTEM_PROCESSOR arm )
set( CMAKE_C_COMPILER arm-linux-gnueabihf-gcc )
set( CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++ )
set( CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install )


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=armv7-a")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall   -O3 -march=armv7-a")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s" )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s" )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-DCOMPILEDWITHC11)
message(STATUS "Using flag -std=c++11.")

include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/Thirdparty
${PROJECT_SOURCE_DIR}/include
${PROJECT_SOURCE_DIR}/Thirdparty/eigen3
${PROJECT_SOURCE_DIR}/Thirdparty/OpenCV
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)


add_library(ORB_SLAM2_FPGA SHARED
src/System.cc
src/Tracking.cc
src/LocalMapping.cc
src/LoopClosing.cc
src/FPGAORBextractor.cc
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

# Build examples

link_directories(${PROJECT_SOURCE_DIR}/Thirdparty/OpenCV/lib/
                 ${PROJECT_SOURCE_DIR}/Thirdparty/cma/lib/)
set(OPENCV_LIBS -lopencv_dnn -lopencv_highgui -lopencv_ml -lopencv_objdetect -lopencv_shape 
                -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_videoio 
                -lopencv_imgcodecs -lopencv_features2d -lopencv_video -lopencv_photo -lopencv_imgproc 
                -lopencv_flann -lopencv_core
                -lavcodec -lavdevice -lavfilter -lavformat -lavutil
                -lswresample -lswscale -lx264)
set(CMA_LIBS ${PROJECT_SOURCE_DIR}/Thirdparty/cma/lib/libcma.so -lpthread)

target_link_libraries(ORB_SLAM2_FPGA
   ${PROJECT_SOURCE_DIR}/Thirdparty/DBoW2/lib/libDBoW2.so
   ${PROJECT_SOURCE_DIR}/Thirdparty/g2o/lib/libg2o.so
   ${PROJECT_SOURCE_DIR}/Thirdparty/OpenCV/lib/libopencv_dnn.so
   ${PROJECT_SOURCE_DIR}/Thirdparty/cma/lib/libcma.so
  #  ${CMA_LIBS}
   ${PROJECT_SOURCE_DIR}/lib/libDmaDriver.so
)

add_library(DmaDriver SHARED
src/fpga/DmaDriver.cc
)
set_target_properties(DmaDriver PROPERTIES COMPILE_FLAGS "-O3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/RGB-D)

# add_executable(rgbd_tum
# Examples/RGB-D/rgbd_tum.cc)
# target_link_libraries(rgbd_tum ${PROJECT_NAME} ${OPENCV_LIBS})

add_executable(rgbd_tum_fpga
Examples/RGB-D/rgbd_tum_fpga.cc)
target_link_libraries(rgbd_tum_fpga ORB_SLAM2_FPGA ${OPENCV_LIBS})


# tests
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/myExample)
# add_executable(orbslam_test ${PROJECT_SOURCE_DIR}/Examples/myExample/orbslam_test.cpp)
# target_link_libraries(orbslam_test ${PROJECT_NAME} ${OPENCV_LIBS})

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/dma_test)
# add_executable(dma_test ${PROJECT_SOURCE_DIR}/Examples/dma_test/dma_test.c)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/cma_test)
# add_executable(cma_test ${PROJECT_SOURCE_DIR}/Examples/cma_test/cma_test.c)
# target_link_libraries(cma_test ${CMA_LIBS})

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/fpga_orb_test)
# set(CMAKE_xxx_CXX_FLAGS "-O0")
# add_executable(fpga_orb_test_old ./Examples/fpga_orb_test/fpga_orb_test_old.cpp)
# set_target_properties(fpga_orb_test_old PROPERTIES COMPILE_FLAGS ${CMAKE_xxx_CXX_FLAGS})
# target_link_libraries(fpga_orb_test_old ${OPENCV_LIBS} ${CMA_LIBS})

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/fpga_orb_test)
# add_executable(fpga_orb_test ./Examples/fpga_orb_test/fpga_orb_test_2.cpp)
# set_target_properties(fpga_orb_test PROPERTIES COMPILE_FLAGS "-O3")
# target_link_libraries(fpga_orb_test ${OPENCV_LIBS} ${CMA_LIBS} DmaDriver)
