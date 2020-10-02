cmake_minimum_required(VERSION 2.8)
project(DBoW2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=armv7-a")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall  -O3 -march=armv7-a")

set(HDRS_DBOW2
  DBoW2/BowVector.h
  DBoW2/FORB.h 
  DBoW2/FClass.h       
  DBoW2/FeatureVector.h
  DBoW2/ScoringObject.h   
  DBoW2/TemplatedVocabulary.h)
set(SRCS_DBOW2
  DBoW2/BowVector.cpp
  DBoW2/FORB.cpp      
  DBoW2/FeatureVector.cpp
  DBoW2/ScoringObject.cpp)

set(HDRS_DUTILS
  DUtils/Random.h
  DUtils/Timestamp.h)
set(SRCS_DUTILS
  DUtils/Random.cpp
  DUtils/Timestamp.cpp)

# LINK_DIRECTORIES(~/SLAM_PYNQ/libs/opencv)

find_package(OpenCV 3.0 QUIET)
if(NOT OpenCV_FOUND)
   find_package(OpenCV 2.4.3 QUIET)
   if(NOT OpenCV_FOUND)
      message(FATAL_ERROR "OpenCV > 2.4.3 not found.")
   endif()
endif()

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${OpenCV_INCLUDE_DIRS})
add_library(DBoW2 SHARED ${SRCS_DBOW2} ${SRCS_DUTILS})

set(OPENCV_DYNAMIC_LIBS  "/home/magicstudio/SLAM_ZYNQ/libs/opencv/lib")       # 动态 opencv native 库路径
add_library(libopencv_dnn SHARED IMPORTED )
set_target_properties(libopencv_dnn PROPERTIES   IMPORTED_LOCATION "/home/magicstudio/SLAM_ZYNQ/libs/opencv/lib/libopencv_dnn.so")

set(mOpenCV_LIBS libopencv_dnn)

target_link_libraries(DBoW2 ${mOpenCV_LIBS})

