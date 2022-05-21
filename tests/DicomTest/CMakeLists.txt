project (DicomTest)

set (PROJECT DicomTest)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(include)
include_directories(../modules/Dicom/)

enable_testing()

set (HEADERS 
             )

set (SOURCES src/TagStruct/ParseHelpers_tests.cpp
             src/main.cpp
             src/CastValueTests.cpp
             src/MVectorTests.cpp
             )

include(GoogleTest)

add_executable(${PROJECT} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} Dicom gtest_main)
