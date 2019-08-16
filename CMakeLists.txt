cmake_minimum_required(VERSION 2.8)

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

#Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

set (Boost_INCLUDE_DIR ${BOOST_DIR})
set (Boost_LIBRARY_DIR ${BOOST_DIR}/stage/lib)

set (BOOST_COMPONENTS )  #filesystem program_options iostreams timer)

find_package (Boost COMPONENTS ${BOOST_COMPONENTS} REQUIRED)

include_directories(PRIVATE ${Boost_INCLUDE_DIR})
include_directories(PRIVATE ${Boost_INCLUDE_DIR})

if (CMAKE_COMPILER_IS_GNUCXX)
set (LIBRARIES ${LIBRARIES}
               gomp)
endif ()

#Modules
add_subdirectory(modules/Dicom)

#tests
add_subdirectory(tests/googletest)
add_subdirectory(tests/DicomTest)

add_executable (${PROJECT} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT} ${LIBRARIES})
target_include_directories(${PROJECT} PRIVATE ${Boost_INCLUDE_DIRS})
