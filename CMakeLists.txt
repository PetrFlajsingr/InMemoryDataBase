cmake_minimum_required(VERSION 3.8)

#if (APPLE)
#    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
#    add_definitions(-D__GLIBCXX__)
#endif (APPLE)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})

add_subdirectory(third_party/google_test)

include_directories(third_party/google_test/googletest/include)

include_directories(third_party/google_test/googlemock/include)

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

project(csv_reader)

#if (APPLE)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g ")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -g ")
#endif (APPLE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -o3")

set(SOURCE_FILES
        Misc/Headers/DateTimeUtils.h Datasets/Fields/Headers/DateTimeField.h
        Datasets/Fields/Source/DateTimeField.cpp DataWorkers/Source/DataSetMerger.cpp
        DataWorkers/Headers/DataSetMerger.h DataProviders/Source/CsvReader.cpp DataProviders/Headers/CsvReader.h
        DataProviders/Headers/BaseDataProvider.h Misc/Headers/Exceptions.h Misc/Source/Utilities.cpp
        Misc/Headers/Utilities.h Datasets/Headers/BaseDataSet.h Datasets/Source/MemoryDataSet.cpp
        Datasets/Headers/MemoryDataSet.h Misc/Headers/Types.h Datasets/Fields/Headers/BaseField.h
        Datasets/Fields/Source/StringField.cpp Datasets/Fields/Headers/StringField.h
        Datasets/Fields/Source/IntegerField.cpp Datasets/Fields/Headers/IntegerField.h
        Datasets/Fields/Source/DoubleField.cpp Datasets/Fields/Headers/DoubleField.h Datasets/Source/BaseDataSet.cpp
        Datasets/Fields/Source/BaseField.cpp Datasets/Headers/FilterStructures.h
        DataProviders/Headers/ArrayDataProvider.h
        Datasets/Source/Async_MemoryDataSet.cpp Datasets/Headers/Async_MemoryDataSet.h
        DataWriters/Source/CsvWriter.cpp DataWriters/Headers/CsvWriter.h FileDownloaders/Headers/BaseFileDownloader.h
        FileDownloaders/Headers/FileDownloadObserver.h third_party/decimal/Decimal.h
        Datasets/Fields/Source/CurrencyField.cpp Datasets/Fields/Headers/CurrencyField.h
        DataWorkers/Headers/BaseDataWorker.h DataWorkers/Source/MemoryDataWorker.cpp DataWorkers/Headers/MemoryDataWorker.h
        DataWriters/Headers/BaseDataWriter.h DataWriters/Source/ArrayWriter.cpp
        DataWriters/Headers/ArrayWriter.h Datasets/Headers/SortStructures.h Misc/Headers/Logger.h cedr.cpp)

set(TEST_SOURCE_FILES
        RelationLayer/Source/RelationBuilder.cpp RelationLayer/Source/RelationManager.cpp
        RelationLayer/Headers/RelationBuilder.h RelationLayer/Headers/RelationManager.h
        RelationLayer/Headers/Relation.h RelationLayer/Source/Relation.cpp DataProviders/tests/tests_XlsReader.cpp
        DataProviders/Headers/XlsReader.h DataProviders/Source/XlsReader.cpp Misc/Headers/DateTimeUtils.h
        Datasets/Fields/Headers/DateTimeField.h Datasets/Fields/Source/DateTimeField.cpp
        third_party/xlsxio/include/xlsxio_read.h third_party/xlsxio/include/xlsxio_write.h third_party/xlsxio/include/xlsxio_version.h
        DataWriters/tests/tests_XslxWriter.cpp DataWriters/Headers/XlsxWriter.h DataWriters/Source/XlsxWriter.cpp
        DataWorkers/tests/tests_DataSetMerger.cpp DataWorkers/Source/DataSetMerger.cpp
        DataWorkers/Headers/DataSetMerger.h DataProviders/Source/CsvReader.cpp DataProviders/Headers/CsvReader.h
        DataProviders/Headers/BaseDataProvider.h Misc/Headers/Exceptions.h Misc/Source/Utilities.cpp
        Misc/Headers/Utilities.h Datasets/Headers/BaseDataSet.h Datasets/Source/MemoryDataSet.cpp
        Datasets/Headers/MemoryDataSet.h Misc/Headers/Types.h Datasets/Fields/Headers/BaseField.h
        Datasets/Fields/Source/StringField.cpp Datasets/Fields/Headers/StringField.h
        Datasets/Fields/Source/IntegerField.cpp Datasets/Fields/Headers/IntegerField.h
        Datasets/Fields/Source/DoubleField.cpp Datasets/Fields/Headers/DoubleField.h Datasets/Source/BaseDataSet.cpp
        Datasets/Fields/Source/BaseField.cpp Datasets/Headers/FilterStructures.h
        Misc/tests/tests_isDouble_isInteger.cpp Misc/tests/tests_stringToInt_Double.cpp
        DataProviders/tests/tests_CsvReader.cpp Datasets/tests/tests_MemoryDataset.cpp
        DataProviders/Headers/ArrayDataProvider.h
        Datasets/Source/Async_MemoryDataSet.cpp Datasets/Headers/Async_MemoryDataSet.h
        Datasets/tests/tests_AsyncMemoryDataset.cpp
        DataWriters/Source/CsvWriter.cpp DataWriters/Headers/CsvWriter.h FileDownloaders/Headers/BaseFileDownloader.h
        FileDownloaders/Headers/FileDownloadObserver.h third_party/decimal/Decimal.h
        Datasets/Fields/Source/CurrencyField.cpp Datasets/Fields/Headers/CurrencyField.h
        DataWorkers/Headers/BaseDataWorker.h DataWorkers/Source/MemoryDataWorker.cpp DataWorkers/Headers/MemoryDataWorker.h
        DataWorkers/tests/tests_DataWorkers.cpp DataWriters/Headers/BaseDataWriter.h DataWriters/Source/ArrayWriter.cpp
        DataWriters/Headers/ArrayWriter.h Datasets/Headers/SortStructures.h
        DataWorkers/tests/tests_SQLParser.cpp Misc/Headers/Logger.h)


add_executable(cedr cedr.cpp ${SOURCE_FILES})
add_executable(csv_reader_test ${TEST_SOURCE_FILES})

target_link_libraries(csv_reader_test gmock_main)
target_link_libraries(csv_reader_test xlsxio_read)
target_link_libraries(csv_reader_test xlsxio_write)


