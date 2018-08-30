cmake_minimum_required(VERSION 3.8)

if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

project(csv_reader)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES DataProviders/CsvReader.cpp DataProviders/CsvReader.h Interfaces/IDataProvider.h Misc/Exceptions.h Misc/Utilities.cpp Misc/Utilities.h Interfaces/IDataset.h Datasets/Memorydataset.cpp Datasets/Memorydataset.h Misc/Types.h Interfaces/IField.h Datasets/Fields/StringField.cpp Datasets/Fields/StringField.h Datasets/Fields/IntegerField.cpp Datasets/Fields/IntegerField.h Datasets/Fields/DoubleField.cpp Datasets/Fields/DoubleField.h Interfaces/IFieldPre.h Interfaces/IDataset.cpp Interfaces/IDatasetPre.h Interfaces/IField.cpp Datasets/FilterStructures.h Misc/tests/tests_splitStringByDelimiter.h)
set(TEST_SOURCE_FILES Tests.cpp DataProviders/CsvReader.cpp DataProviders/CsvReader.h Interfaces/IDataProvider.h Misc/Exceptions.h Misc/Utilities.cpp Misc/Utilities.h Interfaces/IDataset.h Datasets/Memorydataset.cpp Datasets/Memorydataset.h Misc/Types.h Interfaces/IField.h Datasets/Fields/StringField.cpp Datasets/Fields/StringField.h Datasets/Fields/IntegerField.cpp Datasets/Fields/IntegerField.h Datasets/Fields/DoubleField.cpp Datasets/Fields/DoubleField.h Interfaces/IFieldPre.h Interfaces/IDataset.cpp Interfaces/IDatasetPre.h Interfaces/IField.cpp Datasets/FilterStructures.h Misc/tests/tests_isDouble_isInteger.h)


add_subdirectory(lib/googletest-master)

include_directories(lib/googletest-master/googletest/include)

include_directories(lib/googletest-master/googlemock/include)



add_executable(csv_reader main.cpp ${SOURCE_FILES})
add_executable(csv_reader_test Tests.cpp ${TEST_SOURCE_FILES})

target_link_libraries(csv_reader_test gtest gtest_main)


