

#include <iostream>
#include <iomanip>
#include <sys/time.h>
#include "Interfaces/IDataset.h"
#include "Interfaces/IField.h"
#include "Datasets/Memorydataset.h"
#include "DataProviders/CsvReader.h"

void printTime(){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << std::endl;
}

long getMs() {
    struct timeval tp{};
    gettimeofday(&tp, nullptr);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

int main(int argc, char** argv) {


    auto * dataset = new Memorydataset();

    IDataProvider* dataProvider = new CsvReader(argv[1]);

    dataset->setDataProvider(dataProvider);
    std::vector<ValueType> types;
    for(int iter = 0; iter < dataProvider->getColumnCount(); iter++){
        types.push_back(STRING_VAL);
    }
    dataset->setFieldTypes(types);

    long start = getMs();
    std::cout << "_____Open()_____" << std::endl;
    printTime();
    dataset->open();
    std::cout << "_____Open() done_____ time: " << getMs() - start << " ms" << std::endl;
    printTime();


    auto field0 = dataset->fieldByIndex(0);
    auto field1 = dataset->fieldByIndex(4);

    std::cout << "_____sort()_____" << std::endl;
    start = getMs();
    dataset->sort(1, ASCENDING);
    std::cout << "_____sort() end_____ time: " << getMs() - start << " ms" << std::endl;
    printTime();


    Memorydataset::SearchOptions options;
    options.addOption(4, "00231525");
    start = getMs();
    std::cout << "_____find()_____" << std::endl;
    printTime();
    dataset->find(options);
    std::cout << "_____find() end_____ time: " << getMs() - start << " ms" << std::endl;
    printTime();


    std::cout << "_____next()_____" << std::endl;
    printTime();
    start = getMs();
    while(!dataset->eof()) {
        std::cout << field0->getAsString() << " " << field1->getAsString() << std::endl;
        dataset->next();
    }
    std::cout << "_____next() done_____ time: " << getMs() - start << " ms" << std::endl;
    printTime();

    dataset->close();

    delete dataset;

    return 0;
}