cmake_minimum_required(VERSION 2.8)

project(checkFaceApi)

set(HEADERS_SHARED
    "include/utils.h"
    "include/face_api.h"
    "include/in_out.h"
)

set(SOURCES_SHARED
    "src/utils.cpp"
    "src/in_out.cpp"
)

set(HEADERS_V
    "include/utils_V.h"
    "include/face_api_V.h"
    "include/timing.h"
    "include/in_out_V.h"
    "include/face_api_example_V.h"
)

set(SOURCES_V
    "src/main_V.cpp"
    "src/utils_V.cpp"
    "src/face_api_V.cpp"
    "src/timing.cpp"
    "src/in_out_V.cpp"
    "src/face_api_example_V.cpp"
)

set(HEADERS_I
    "include/utils_I.h"
    "include/face_api_I.h"
    "include/timing.h"
    "include/in_out_I.h"
    "include/face_api_example_I.h"
)

set(SOURCES_I
    "src/main_I.cpp"
    "src/utils_I.cpp"
    "src/face_api_I.cpp"
    "src/timing.cpp"
    "src/in_out_I.cpp"
    "src/face_api_example_I.cpp"
)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_INSTALL_RPATH "$ORIGIN")

find_package(OpenCV REQUIRED)
set(FACE_API_INCLUDE_DIR ${FACE_API_ROOT_DIR}/include/)
set(FACE_API_LIB_DIR ${FACE_API_ROOT_DIR}/lib/)

if(FREEIMAGE_ROOT_DIR)
    set(FREEIMAGE_INCLUDE_DIR ${FREEIMAGE_ROOT_DIR}/include/)
    set(FREEIMAGE_LIB_DIR ${FREEIMAGE_ROOT_DIR}/lib/)
    set(FREEIMAGE_LIBRARIES libFreeImage.a libjpeg.a libMetadata.a
                            libpng16.a libz.a libFreeImageToolkit.a libLibTIFF4.a)
else()
    set(FREEIMAGE_LIBRARIES freeimage)
endif()

include_directories("include" ${FACE_API_INCLUDE_DIR} ${TCLAP_INCLUDE_DIR} ${FREEIMAGE_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})

link_directories(${FACE_API_LIB_DIR} ${FREEIMAGE_LIB_DIR})

add_executable(${PROJECT_NAME}_V ${HEADERS_SHARED} ${HEADERS_V} ${SOURCES_SHARED} ${SOURCES_V})
add_executable(${PROJECT_NAME}_I ${HEADERS_SHARED} ${HEADERS_I} ${SOURCES_SHARED} ${SOURCES_I})

target_link_libraries(${PROJECT_NAME}_V glog pthread ${FREEIMAGE_LIBRARIES} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME}_I glog pthread ${FREEIMAGE_LIBRARIES} ${OpenCV_LIBS})

install(TARGETS ${PROJECT_NAME}_V ${PROJECT_NAME}_I DESTINATION .)
