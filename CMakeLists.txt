cmake_minimum_required(VERSION 2.8.2)

set(CMAKE_CXX_STANDARD 17)

set (PROJECT Dcm)

project (${PROJECT})

include_directories(include/)
include_directories(modules/Dicom/include)


set (HEADERS
)

set (SOURCES
     src/main.cpp
)

set (LIBRARIES
     Dicom
)

if (CMAKE_COMPILER_IS_GNUCXX)
set (LIBRARIES ${LIBRARIES}
               gomp)
endif ()

#Modules
add_subdirectory(modules/Dicom)

#tests
add_subdirectory(tests)

add_executable (${PROJECT} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT} ${LIBRARIES})
