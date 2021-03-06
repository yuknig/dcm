project (Dicom)

set (PROJECT Dicom)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set (HEADERS 
             include/Dicom/TagStruct/SingleValue.h
             include/Dicom/TagStruct/TagNum.h
             include/Dicom/TagStruct/Vr.h
             include/Dicom/dict.h
             include/Dicom/Parser.h
             include/Dicom/TagValues.h
             include/Dicom/Util.h
             include/Dicom/Util.inl
             include/Util/buffer_view.h
             include/Util/CastValue.h
             include/Util/optional.h
             include/Util/Stream.h
             include/Util/Stream.inl
             include/Util/StreamMem.h
             include/Util/StreamFile.h
             include/Util/string_util.h
             include/Util/MVector.h
             )

set (SOURCES 
             src/TagNum.cpp
             src/Parser.cpp
             src/dict.cpp
             src/TagValues.cpp
             src/Vr.cpp
             src/Stream.cpp)

include_directories(PRIVATE ${Boost_INCLUDE_DIRS})

set (LIBRARIES
#    pugixml
)

add_library(${PROJECT} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT} ${LIBRARIES} ${Boost_LIBRARIES})
#target_include_directories(${PROJECT} PRIVATE ${Boost_INCLUDE_DIRS})
