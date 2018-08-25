cmake_minimum_required(VERSION 3.8)
project(csv_reader)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp DataProviders/CsvReader.cpp DataProviders/CsvReader.h Interfaces/IDataProvider.h Misc/Exceptions.h Misc/Utilities.cpp Misc/Utilities.h DataAcessors/DataAccessor.cpp DataAcessors/DataAccessor.h Interfaces/IDataset.h Datasets/Field.cpp Datasets/Field.h Datasets/InMemorydataset.cpp Datasets/InMemorydataset.h Misc/Types.h)
add_executable(csv_reader ${SOURCE_FILES})