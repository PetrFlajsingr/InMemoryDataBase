cmake_minimum_required(VERSION 3.8)

if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})


add_subdirectory(lib/googletest-master)

include_directories(lib/googletest-master/googletest/include)

include_directories(lib/googletest-master/googlemock/include)

include_directories(Misc/Headers)

include_directories(Datasets/Headers)

include_directories(DataProviders/Headers)

include_directories(DataWriters/Headers)

include_directories(Datasets/Fields/Headers)

include_directories(FileDownloaders/Headers)

project(csv_reader)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -g ")

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES DataProviders/Source/CsvReader.cpp DataProviders/Headers/CsvReader.h DataProviders/Headers/BaseDataProvider.h Misc/Headers/Exceptions.h
        Misc/Source/Utilities.cpp Misc/Headers/Utilities.h Datasets/Headers/BaseDataSet.h Datasets/Source/MemoryDataSet.cpp Datasets/Headers/MemoryDataSet.h
        Misc/Headers/Types.h Datasets/Fields/Headers/BaseField.h Datasets/Fields/Source/StringField.cpp Datasets/Fields/Headers/StringField.h
        Datasets/Fields/Source/IntegerField.cpp Datasets/Fields/Headers/IntegerField.h Datasets/Fields/Source/DoubleField.cpp
        Datasets/Fields/Headers/DoubleField.h Datasets/Source/BaseDataSet.cpp
        Datasets/Fields/Source/BaseField.cpp Datasets/Headers/FilterStructures.h Misc/tests/tests_splitStringByDelimiter.cpp
        Datasets/Source/Async_MemoryDataSet.cpp Datasets/Headers/Async_MemoryDataSet.h)

set(TEST_SOURCE_FILES DataProviders/Source/CsvReader.cpp DataProviders/Headers/CsvReader.h DataProviders/Headers/BaseDataProvider.h
        Misc/Headers/Exceptions.h Misc/Source/Utilities.cpp Misc/Headers/Utilities.h Datasets/Headers/BaseDataSet.h Datasets/Source/MemoryDataSet.cpp
        Datasets/Headers/MemoryDataSet.h Misc/Headers/Types.h Datasets/Fields/Headers/BaseField.h Datasets/Fields/Source/StringField.cpp
        Datasets/Fields/Headers/StringField.h Datasets/Fields/Source/IntegerField.cpp Datasets/Fields/Headers/IntegerField.h
        Datasets/Fields/Source/DoubleField.cpp Datasets/Fields/Headers/DoubleField.h Datasets/Source/BaseDataSet.cpp
        Datasets/Fields/Source/BaseField.cpp Datasets/Headers/FilterStructures.h Misc/tests/tests_isDouble_isInteger.cpp
        Misc/tests/tests_stringToInt_Double.cpp DataProviders/tests/tests_CsvReader.cpp Datasets/tests/tests_MemoryDataset.cpp
        DataProviders/Headers/ArrayDataProvider.h
        Datasets/Source/Async_MemoryDataSet.cpp Datasets/Headers/Async_MemoryDataSet.h Datasets/tests/tests_AsyncMemoryDataset.cpp
        DataWriters/Source/CsvWriter.cpp DataWriters/Headers/CsvWriter.h FileDownloaders/Headers/BaseFileDownloader.h FileDownloaders/Headers/FileDownloadObserver.h)


add_executable(csv_reader main.cpp ${SOURCE_FILES})
add_executable(csv_reader_test ${TEST_SOURCE_FILES})

target_link_libraries(csv_reader_test gmock_main)


