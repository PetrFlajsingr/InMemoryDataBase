

#include <iostream>
#include <iomanip>
#include "Interfaces/IDataset.h"
#include "Datasets/InMemorydataset.h"
#include "DataProviders/CsvReader.h"

void printTime(){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << std::endl;
}

int main(int argc, char** argv) {
    IDataset* dataset = new InMemorydataset();

    IDataProvider* dataProvider = new CsvReader(argv[1]);

    dataset->setDataProvider(dataProvider);
    std::vector<ValueType> types;
    for(int iter = 0; iter < dataProvider->getColumnCount(); iter++){
        types.push_back(STRING_VAL);
    }
    dataset->setFieldTypes(types);
    std::cout << "_____Open()_____" << std::endl;
    printTime();
    dataset->open();
    std::cout << "_____Open() done_____" << std::endl;
    printTime();


    auto field0 = dataset->fieldByIndex(0);
    auto field1 = dataset->fieldByIndex(1);

    std::cout << "_____next()_____" << std::endl;
    printTime();
    while(!dataset->eof()) {
        std::cout << field0->getAsString() << " " << field1->getAsString() << std::endl;
        dataset->next();
    }
    std::cout << "_____next() done_____" << std::endl;
    printTime();

    dataset->close();

    delete dataset;

    return 0;
}