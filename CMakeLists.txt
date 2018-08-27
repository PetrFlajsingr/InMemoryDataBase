cmake_minimum_required(VERSION 3.8)
project(csv_reader)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp DataProviders/CsvReader.cpp DataProviders/CsvReader.h Interfaces/IDataProvider.h Misc/Exceptions.h Misc/Utilities.cpp Misc/Utilities.h Interfaces/IDataset.h Datasets/Memorydataset.cpp Datasets/Memorydataset.h Misc/Types.h Interfaces/IField.h Datasets/Fields/StringField.cpp Datasets/Fields/StringField.h Datasets/Fields/IntegerField.cpp Datasets/Fields/IntegerField.h Datasets/Fields/DoubleField.cpp Datasets/Fields/DoubleField.h Interfaces/IFieldPre.h Interfaces/IDataset.cpp Interfaces/IDatasetPre.h Interfaces/IField.cpp)
add_executable(csv_reader ${SOURCE_FILES})