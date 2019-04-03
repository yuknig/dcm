project (Dicom)

set (PROJECT Dicom)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set (HEADERS include/Dicom/TagStruct/TagNum.h
             include/Dicom/Parser.h
             include/Dicom/Vr.h
             include/Dicom/TagValues.h
             include/Dicom/TagValues.inl
             include/Dicom/Tags.h
             include/Dicom/Util.h
             include/Dicom/Util.inl
             include/Util/CastValue.h
             include/Util/CastValue.inl
             include/Util/optional.h
             include/Util/Stream.h
             include/Util/Stream.inl
             include/Util/StreamMem.h
             include/Util/StreamMem.inl
             include/Util/MVector.h
             )

set (SOURCES src/TagNum.cpp
             src/Parser.cpp
             src/TagValues.cpp
             src/Vr.cpp
             src/Stream.cpp)

#Boost
#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREADED ON)

#set (Boost_INCLUDE_DIR F:/develop/boost/boost_1_64_0)
#set (Boost_LIBRARY_DIR F:/develop/boost/boost_1_64_0/stage/lib)

#set (BOOST_COMPONENTS filesystem program_options iostreams timer)

#find_package (Boost COMPONENTS ${BOOST_COMPONENTS} REQUIRED)

#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

#message(STATUS "Boost_INCLUDE_DIR    : ${Boost_INCLUDE_DIR}")
#message(STATUS "Boost_INCLUDE_DIRS   : ${Boost_INCLUDE_DIRS}")

include_directories(PRIVATE ${Boost_INCLUDE_DIRS})

set (LIBRARIES
#    pugixml
)

add_library(${PROJECT} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT} ${LIBRARIES} ${Boost_LIBRARIES})
#target_include_directories(${PROJECT} PRIVATE ${Boost_INCLUDE_DIRS})
#target_include_directories(${PROJECT} PRIVATE ${Boost_INCLUDE_DIR})
