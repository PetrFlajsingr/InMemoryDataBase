cmake_minimum_required(VERSION 3.8)
project(csv_reader)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp CsvReader.cpp CsvReader.h IReader.h Exceptions.h DataContainer.cpp DataContainer.h Utilities.cpp Utilities.h DataAccessor.cpp DataAccessor.h)
add_executable(csv_reader ${SOURCE_FILES})