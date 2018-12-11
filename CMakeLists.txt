cmake_minimum_required(VERSION 2.8)
project( PNP_DEMO )

include_directories(${OpenCV_INCLUDE_DIRS})

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/opt/local/lib/cmake/OpenCVConfig.cmake)
set(CMAKE_MODULE_PATH "FindOpenCV.cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_PREFIX_PATH "/usr/local/opt/opencv3/share/OpenCV")

set(OpenCV_INCLUDE_DIRS "/usr/local/opt/opencv3/include")
set(OpenCV_LIBS "/usr/local/opt/opencv3/lib")

set(OPENCV_ROOT "/build/x86/vc11/lib") ##
set(OPENCV_ROOT "/x86/vc11/lib/include/opencv")
set(OpenCV_DIR=/home/ubuntu/opencv-3.2.0/share/OpenCV)

set(SOURCE_FILES main.cpp)
find_package(OpenCV REQUIRED)
#ADD_SUBDIRECTORY('C:\tools')
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/tools)

find_package(OpenCV HINTS /usr/local/opt/opencv3)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

add_executable(cv_project1 main.cpp)

target_link_libraries(cv_project1 ${OpenCV_LIBS})
add_library(pnp_lib
    src/CsvReader.cpp
    src/CsvWriter.cpp
    src/ModelRegistration.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/PnPProblem.cpp
    src/Utils.cpp
    src/RobustMatcher.cpp
    src/kalman_filter_tracker.cpp)

add_executable( pnp_registration src/main_registration.cpp )
add_executable( pnp_detection src/main_detection.cpp )
add_executable( pnp_test src/test_pnp.cpp )

target_link_libraries( pnp_registration pnp_lib ${OpenCV_LIBS} )
target_link_libraries( pnp_detection pnp_lib ${OpenCV_LIBS} )
target_link_libraries( pnp_test pnp_lib ${OpenCV_LIBS} )
