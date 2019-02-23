cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -o3")

if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

include_directories(third_party)

include_directories(Misc/tests)

include_directories(Misc/Headers)

include_directories(DataSets/Headers)

include_directories(DataProviders/Headers)

include_directories(DataWriters/Headers)

include_directories(DataSets/Fields/Headers)

include_directories(FileDownloaders/Headers)

include_directories(third_party/Decimal)

include_directories(DataWorkers/Headers)

include_directories(DataBase/Headers)

include_directories(DataBase/QueryParser/Headers)

include_directories(third_party/xlsxio/include)

include_directories(third_party/catch2)

FILE(GLOB DataProviderSources DataProviders/Source/*.cpp)
FILE(GLOB DataProviderHeaders DataProviders/Headers/*.h)
FILE(GLOB DataProviderTests DataProviders/tests/*catch*.cpp)
FILE(GLOB DataProviderTestsG DataProviders/tests/Gtests*.cpp)

FILE(GLOB DataSetSources DataSets/Source/*.cpp)
FILE(GLOB DataSetHeaders DataSets/Headers/*.h)
FILE(GLOB DataSetTests DataSets/tests/*catch*.cpp)
FILE(GLOB DataSetTestsG DataSets/tests/Gtests*.cpp)

FILE(GLOB FieldSources DataSets/Fields/Source/*.cpp)
FILE(GLOB FieldHeaders DataSets/Fields/Headers/*.h)

FILE(GLOB DataWorkerSources DataWorkers/Source/*.cpp)
FILE(GLOB DataWorkerHeaders DataWorkers/Headers/*.h)
FILE(GLOB DataWorkerTests DataWorkers/tests/*catch*.cpp)
FILE(GLOB DataWorkerTestsG DataWorkers/tests/Gtests*.cpp)

FILE(GLOB DataWriterSources DataWriters/Source/*.cpp)
FILE(GLOB DataWriterHeaders DataWriters/Headers/*.h)
FILE(GLOB DataWriterTests DataWriters/tests/*catch*.cpp)
FILE(GLOB DataWriterTestsG DataWriters/tests/Gtests*.cpp)

FILE(GLOB MiscSources Misc/Source/*.cpp)
FILE(GLOB MiscHeaders Misc/Headers/*.h)
FILE(GLOB MiscTests Misc/tests/*catch*.cpp)
FILE(GLOB MiscTestsG Misc/tests/Gtests*.cpp)

FILE(GLOB RelationLayerSources DataBase/Source/*.cpp)
FILE(GLOB RelationLayerHeaders DataBase/Headers/*.h)
FILE(GLOB RelationLayerTests DataBase/tests/*catch*.cpp)

FILE(GLOB FileDownloaderSources FileDownloaders/Source/*.cpp)
FILE(GLOB FileDownloaderHeaders FileDownloaders/Headers/*.h)

FILE(GLOB QueryParserSources DataBase/QueryParser/Source/*.cpp)
FILE(GLOB QueryParserHeaders DataBase/QueryParser/Headers/*.h)

# third party:
FILE(GLOB xlsxioHeaders third_party/xlsxio/include/*.h)
FILE(GLOB decimalHeaders third_party/decimal/*.h)

set(SOURCE_FILES
        ${DataProviderSources}
        ${DataSetSources}
        ${QueryParserSources}
        ${FieldSources}
        ${DataWorkerSources}
        ${DataWriterSources}
        ${MiscSources}
        ${RelationLayerSources}
        ${FileDownloaderSources})

set(TEST_SOURCE_FILES
        ${SOURCE_FILES}
        ${DataProviderTests}
        ${DataSetTests}
        ${DataWorkerTests}
        ${DataWriterTests}
        ${MiscTests}
        ${RelationLayerTests})

set(GTEST_SOURCE_FILES
        ${SOURCE_FILES}
        ${DataProviderTestsG}
        ${DataSetTestsG}
        ${DataWorkerTestsG}
        ${DataWriterTestsG}
        ${MiscTestsG})


add_executable(cedr cedr.cpp ${SOURCE_FILES})
add_executable(test test.cpp ${SOURCE_FILES})
add_executable(catch_tests ${TEST_SOURCE_FILES})

target_link_libraries(cedr xlsxio_read)
target_link_libraries(cedr xlsxio_write)

target_link_libraries(catch_tests xlsxio_read)
target_link_libraries(catch_tests xlsxio_write)

target_link_libraries(test xlsxio_read)
target_link_libraries(test xlsxio_write)


