cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -o3")

include_directories(Misc/Headers)

include_directories(Datasets/Headers)

include_directories(DataProviders/Headers)

include_directories(DataWriters/Headers)

include_directories(Datasets/Fields/Headers)

include_directories(FileDownloaders/Headers)

include_directories(third_party/Decimal)

include_directories(DataWorkers/Headers)

include_directories(RelationLayer/Headers)

include_directories(third_party/xlsxio/include)

# TODO: replace with catch2:
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})

add_subdirectory(third_party/google_test)

include_directories(third_party/google_test/googlemock/include)
include_directories(third_party/google_test/googletest/include)
FILE(GLOB GoogleTestHeaders third_party/google_test/googletest/include/*.h)
FILE(GLOB GoogleMockHeaders third_party/google_test/googlemock/include/*.h)
########################

FILE(GLOB DataProviderSources DataProviders/Source/*.cpp)
FILE(GLOB DataProviderHeaders DataProviders/Headers/*.h)
FILE(GLOB DataProviderTests DataProviders/tests/*.cpp)

FILE(GLOB DataSetSources Datasets/Source/*.cpp)
FILE(GLOB DataSetHeaders Datasets/Headers/*.h)
FILE(GLOB DataSetTests Datasets/tests/*.cpp)

FILE(GLOB FieldSources Datasets/Fields/Source/*.cpp)
FILE(GLOB FieldHeaders Datasets/Fields/Headers/*.h)

FILE(GLOB DataWorkerSources DataWorkers/Source/*.cpp)
FILE(GLOB DataWorkerHeaders DataWorkers/Headers/*.h)
FILE(GLOB DataWorkerTests DataWorkers/tests/*.cpp)

FILE(GLOB DataWriterSources DataWriters/Source/*.cpp)
FILE(GLOB DataWriterHeaders DataWriters/Headers/*.h)
FILE(GLOB DataWriterTests DataWriters/tests/*.cpp)

FILE(GLOB MiscSources Misc/Source/*.cpp)
FILE(GLOB MiscHeaders Misc/Headers/*.h)
FILE(GLOB MiscTests Misc/tests/*.cpp)

FILE(GLOB RelationLayerSources RelationLayer/Source/*.cpp)
FILE(GLOB RelationLayerHeaders RelationLayer/Headers/*.h)
FILE(GLOB RelationLayerTests RelationLayer/tests/*.cpp)

# third party:
FILE(GLOB xlsxioHeaders third_party/xlsxio/include/*.h)
FILE(GLOB decimalHeaders third_party/decimal/*.h)

set(SOURCE_FILES
        ${DataProviderSources}
        ${DataSetSources}
        ${FieldSources}
        ${DataWorkerSources}
        ${DataWriterSources}
        ${MiscSources}
        ${RelationLayerSources})

set(TEST_SOURCE_FILES
        ${SOURCE_FILES}
        ${DataProviderTests}
        ${DataSetTests}
        ${DataWorkerTests}
        ${DataWriterTests}
        ${MiscTests}
        ${RelationLayerTests})


add_executable(cedr cedr.cpp ${SOURCE_FILES})
add_executable(csv_reader_test ${TEST_SOURCE_FILES})

target_link_libraries(cedr xlsxio_read)
target_link_libraries(cedr xlsxio_write)

target_link_libraries(csv_reader_test gmock_main)
target_link_libraries(csv_reader_test xlsxio_read)
target_link_libraries(csv_reader_test xlsxio_write)


