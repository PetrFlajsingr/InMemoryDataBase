
#include "Interfaces/IDataset.h"
#include "Datasets/InMemorydataset.h"
#include "DataProviders/CsvReader.h"

int main(int argc, char** argv) {
    IDataset* dataset = new InMemorydataset();

    IDataProvider* dataProvider = new CsvReader(argv[1]);

    dataset->open(dataProvider);

    auto fieldNames = dataset->getFieldNames();
    while(!dataset->eof()) {

    }

    dataset->close();

    delete dataset;

    return 0;
}