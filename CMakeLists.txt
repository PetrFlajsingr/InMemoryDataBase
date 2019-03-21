cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

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
include_directories(CLI/Headers)
include_directories(DataBase/Headers)
include_directories(DataBase/QueryParser/Headers)
include_directories(third_party/xlsxio/include)
include_directories(Messages/Headers)
include_directories(Templates/Headers)

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

FILE(GLOB CLISources CLI/Source/*.cpp)
FILE(GLOB CLIHeaders CLI/Headers/*.h)

FILE(GLOB CommandSources Messages/Source/*.cpp)
FILE(GLOB CommandHeaders Messages/Headers/*.h)

FILE(GLOB moorSources third_party/moor/*.cpp)

# third party:
FILE(GLOB xlsxioHeaders third_party/xlsxio/include/*.h)
FILE(GLOB decimalHeaders third_party/decimal/*.h)

set(SOURCE_FILES
        ${DataProviderSources}
        ${DataSetSources}
        ${QueryParserSources}
        ${FieldSources}
        ${DataWriterSources}
        ${MiscSources}
        ${RelationLayerSources}
        ${FileDownloaderSources}
        ${CLISources}
        ${moorSources}
        ${CommandSources})

set(TEST_SOURCE_FILES
        ${SOURCE_FILES}
        ${DataProviderTests}
        ${DataSetTests}
        ${DataWriterTests}
        ${MiscTests}
        ${RelationLayerTests})


FIND_PACKAGE(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})
FIND_PACKAGE(LibArchive REQUIRED)
include_directories(${LibArchive_INCLUDE_DIR})
FIND_PACKAGE(Boost 1.68 COMPONENTS locale date_time filesystem REQUIRED)


add_executable(cedr cedr.cpp ${SOURCE_FILES})
add_executable(test test.cpp ${SOURCE_FILES})
add_executable(zdrav zdrav.cpp ${SOURCE_FILES})
add_executable(catch_tests ${TEST_SOURCE_FILES})

target_link_libraries(cedr xlsxio_read)
target_link_libraries(cedr xlsxio_write)
target_link_libraries(cedr Boost::locale Boost::date_time xlnt Boost::filesystem ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})

target_link_libraries(zdrav xlsxio_read)
target_link_libraries(zdrav xlsxio_write)
target_link_libraries(zdrav Boost::locale Boost::date_time xlnt Boost::filesystem ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})

target_link_libraries(catch_tests xlsxio_read)
target_link_libraries(catch_tests xlsxio_write)
target_link_libraries(catch_tests Boost::locale Boost::date_time xlnt Boost::filesystem ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})

target_link_libraries(test xlsxio_read)
target_link_libraries(test xlsxio_write)
target_link_libraries(test Boost::locale Boost::date_time xlnt Boost::filesystem ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})


