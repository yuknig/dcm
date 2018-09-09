project (DicomTest)

set (PROJECT DicomTest)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(../modules/Dicom/)
include_directories(../googletest/googletest/include/gtest)

set (HEADERS 
             )

set (SOURCES src/main.cpp
             src/MVectorTests.cpp
             )

add_executable(${PROJECT} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} Dicom gtest)
