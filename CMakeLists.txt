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

include_directories(Interfaces/Headers)

include_directories(Datasets/Fields/Headers)

project(csv_reader)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -g ")

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES DataProviders/Sources/CsvReader.cpp DataProviders/Headers/CsvReader.h DataProviders/Headers/BaseDataProvider.h Misc/Headers/Exceptions.h
        Misc/Sources/Utilities.cpp Misc/Headers/Utilities.h Datasets/Headers/BaseDataSet.h Datasets/Sources/MemoryDataSet.cpp Datasets/Headers/MemoryDataSet.h
        Misc/Headers/Types.h Datasets/Fields/Headers/BaseField.h Datasets/Fields/Sources/StringField.cpp Datasets/Fields/Headers/StringField.h
        Datasets/Fields/Sources/IntegerField.cpp Datasets/Fields/Headers/IntegerField.h Datasets/Fields/Sources/DoubleField.cpp
        Datasets/Fields/Headers/DoubleField.h Datasets/Sources/BaseDataSet.cpp
        Datasets/Fields/Sources/BaseField.cpp Datasets/Headers/FilterStructures.h Misc/tests/tests_splitStringByDelimiter.cpp
        Datasets/Sources/Async_MemoryDataSet.cpp Datasets/Headers/Async_MemoryDataSet.h)

set(TEST_SOURCE_FILES DataProviders/Sources/CsvReader.cpp DataProviders/Headers/CsvReader.h DataProviders/Headers/BaseDataProvider.h
        Misc/Headers/Exceptions.h Misc/Sources/Utilities.cpp Misc/Headers/Utilities.h Datasets/Headers/BaseDataSet.h Datasets/Sources/MemoryDataSet.cpp
        Datasets/Headers/MemoryDataSet.h Misc/Headers/Types.h Datasets/Fields/Headers/BaseField.h Datasets/Fields/Sources/StringField.cpp
        Datasets/Fields/Headers/StringField.h Datasets/Fields/Sources/IntegerField.cpp Datasets/Fields/Headers/IntegerField.h
        Datasets/Fields/Sources/DoubleField.cpp Datasets/Fields/Headers/DoubleField.h Datasets/Sources/BaseDataSet.cpp
        Datasets/Fields/Sources/BaseField.cpp Datasets/Headers/FilterStructures.h Misc/tests/tests_isDouble_isInteger.cpp
        Misc/tests/tests_stringToInt_Double.cpp DataProviders/tests/tests_CsvReader.cpp Datasets/tests/tests_MemoryDataset.cpp
        DataProviders/Headers/ArrayDataProvider.h
        Datasets/Sources/Async_MemoryDataSet.cpp Datasets/Headers/Async_MemoryDataSet.h Datasets/tests/tests_AsyncMemoryDataset.cpp)


add_executable(csv_reader main.cpp ${SOURCE_FILES})
add_executable(csv_reader_test ${TEST_SOURCE_FILES})

target_link_libraries(csv_reader_test gmock_main)


