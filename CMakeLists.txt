cmake_minimum_required(VERSION 3.13)

project(csv_reader)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unknown-pragmas -fchar8_t -o3")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

include_directories(third_party)
include_directories(third_party/xlsxio/include)
include_directories(Misc/tests)
include_directories(Misc/Headers)
include_directories(DataSets/Headers)
include_directories(DataProviders/Headers)
include_directories(DataWriters/Headers)
include_directories(DataSets/Fields/Headers)
include_directories(FileDownloaders/Headers)
include_directories(third_party/decimal)
include_directories(CLI/Headers)
include_directories(DataBase/Headers)
include_directories(DataBase/QueryParser/Headers)
include_directories(Messages/Headers)
include_directories(Templates/Headers)
include_directories(utilities)
include_directories(utilities/include)

link_directories(/home/petr/OpenXLSX/build/install/lib)
link_directories(/usr/local/libxls/lib)
link_directories(/usr/local/lib)
include_directories(/usr/local/libxls/include)

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
FILE(GLOB fmtSources utilities/include/fmt/*.cc)

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
        ${CommandSources}
        Combiner.cpp)

set(TEST_SOURCE_FILES
        ${SOURCE_FILES}
        ${DataProviderTests}
        ${DataSetTests}
        ${DataWriterTests}
        ${MiscTests}
        ${RelationLayerTests} DataWriters/Source/BaseDataWriter.cpp)


FIND_PACKAGE(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})
FIND_PACKAGE(LibArchive REQUIRED)
include_directories(${LibArchive_INCLUDE_DIR})

FIND_PACKAGE(Boost COMPONENTS locale date_time filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(cedr_new cedr_new.cpp ${SOURCE_FILES} ${fmtSources})
add_executable(test test.cpp ${SOURCE_FILES} ${fmtSources})
add_executable(zdrav zdrav.cpp ${SOURCE_FILES} ${fmtSources})
add_executable(catch_tests ${TEST_SOURCE_FILES} ${fmtSources})
add_executable(export1 export1.cpp ${SOURCE_FILES} ${fmtSources})
add_executable(export2 export2.cpp ${SOURCE_FILES} ${fmtSources})

FILE(GLOB ARESSources ARES/*.cpp)

add_executable(xml_test xml_test.cpp ${SOURCE_FILES} ${ARESSources})

target_link_libraries(cedr_new xlsxio_read)
target_link_libraries(cedr_new xlsxio_write)
target_link_libraries(cedr_new OpenXLSX xlsreader -pthread Boost::boost Boost::locale Boost::date_time xlnt Boost::filesystem ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})


target_link_libraries(zdrav xlsxio_read)
target_link_libraries(zdrav xlsxio_write)
target_link_libraries(zdrav -pthread Boost::boost Boost::locale Boost::date_time xlnt Boost::filesystem ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})

target_link_libraries(catch_tests xlsxio_read)
target_link_libraries(catch_tests xlsxio_write)
target_link_libraries(catch_tests xlsreader -pthread Boost::boost Boost::locale Boost::date_time xlnt Boost::filesystem ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})

target_link_libraries(test xlsxio_read)
target_link_libraries(test xlsxio_write)
target_link_libraries(test -pthread xlsreader Boost::boost Boost::locale Boost::date_time xlnt Boost::filesystem ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})

target_link_libraries(xml_test libxlsxio_read.dylib)
target_link_libraries(xml_test libxlsxio_write.dylib)
target_link_libraries(xml_test -pthread Boost::boost Boost::locale Boost::date_time libxlnt.dylib Boost::filesystem ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})

target_link_libraries(export1 xlsxio_read)
target_link_libraries(export1 xlsxio_write)
target_link_libraries(export1 xlsreader -pthread Boost::boost Boost::locale Boost::date_time xlnt Boost::filesystem ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})

target_link_libraries(export2 xlsxio_read)
target_link_libraries(export2 xlsxio_write)
target_link_libraries(export2 OpenXLSX xlsreader -pthread Boost::boost Boost::locale Boost::date_time xlnt Boost::filesystem ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})

