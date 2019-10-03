//
// Created by petr on 10/3/19.
//

#include <MemoryDataSet.h>
#include <memory>
#include <Logger.h>
#include <CsvStreamReader.h>
#include <XlsxIOReader.h>
#include <MemoryDataBase.h>

using namespace std::string_literals;

auto logger = Logger<true>::GetInstance();

struct FileSettings {
    enum Type {
        csv, xlsx
    };
    Type type;
    std::string pathToFile;
    char delimiter;
};

std::shared_ptr<DataSets::MemoryDataSet>
createDataSetFromFile(const std::string &name, const FileSettings &fileSettings,
                      const std::vector<ValueType> &valueTypes) {
    std::unique_ptr<DataProviders::BaseDataProvider> provider;
    switch (fileSettings.type) {
        case FileSettings::csv:
            provider = std::make_unique<DataProviders::CsvStreamReader>(fileSettings.pathToFile,
                                                                        fileSettings.delimiter);
            break;
        case FileSettings::xlsx:
            provider = std::make_unique<DataProviders::XlsxIOReader>(fileSettings.pathToFile);
            break;
    }
    auto result = std::make_shared<DataSets::MemoryDataSet>(name);
    result->open(*provider, valueTypes);
    logger.log<LogLevel::Status>("Loaded: "s + result->getName());
    result->resetEnd();
    logger.log<LogLevel::Debug>("Count:", result->getCurrentRecord());
    result->resetBegin();
    return result;
}

const std::string csvPath = "/home/petr/Desktop/cedr/";
const std::string outPath = csvPath + "out/";
const std::string subjektyCSVName = "NNO.xlsx";

int main() {
    DataBase::MemoryDataBase db("db");
    db.addTable(createDataSetFromFile("subjekty", {FileSettings::Type::xlsx, csvPath + subjektyCSVName, ','},
                                                 {ValueType::Integer,
                                                  ValueType::Integer,
                                                  ValueType::String,
                                                  ValueType::String,
                                                  ValueType::String,
                                                  ValueType::String,
                                                  ValueType::String,
                                                  ValueType::String,
                                                  ValueType::String,
                                                  ValueType::String,
                                                  ValueType::String,
                                                  ValueType::String,
                                                  ValueType::String,
                                                  ValueType::String,
                                                  ValueType::String,
                                                  ValueType::String,
                                                  ValueType::String}));

}