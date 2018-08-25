//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef CSV_READER_INMEMORYDATASET_H
#define CSV_READER_INMEMORYDATASET_H


#include "../Interfaces/IDataset.h"

/**
 * Dataset ukladajici data primo v operacni pameti.
 */
class InMemorydataset : IDataset {
private:
    enum SortOrder {ASCENDING, DESCENDING};

    struct SearchOptions {
        std::vector<std::string>    fieldNames;
        std::vector<std::string>    searchString;
    };

    struct SortOptions {
        std::vector<std::string>    fieldNames;
        std::vector<SortOrder>      order;
    };

    /**
     * Struktura pro vnitrni reprezentaci dat
     */
    union InnerData {
        char* _string = nullptr;
        int _integer;
        double _double;
    };

    struct DataContainer {
        ValueType valueType = NONE;
        InnerData data;

        ~DataContainer() {
            if(valueType == STRING_VAL) {
                delete data._string;
            }
        }
    };
    //\

    IDataProvider* dataProvider = nullptr;

    bool isOpen = false;

    std::vector<std::vector<DataContainer>> data;

    void loadData();

    void addRecord();

    /**
     * Vytvoreni Fields se jmeny podle nazvu sloupcu.
     * @param columns nazvy sloupcu
     */
    void createFields(std::vector<std::string> columns);

    void setFieldValues(std::vector<DataContainer> value);

    bool isFieldTypeSet() {
        return !(fields[0].getFieldType() == NONE);
    }

    void setFieldTypes(std::vector<InMemorydataset::DataContainer> value);
public:
    /**
     * Nacteni dat do pameti datasetu.
     * @param provider objekt dodavajici data
     */
    void open(IDataProvider* provider) override;

    /**
     * Smazani dat datasetu z pameti.
     */
    void close() override;

    /**
     * Presun na prvni polozku.
     */
    void first() override;

    /**
     * Presun na posledni polozku.
     */
    void last() override;

    /**
     * Presun na nasledujici polozku.
     */
    void next() override;

    /**
     * Presun na predchazejici polozku.
     */
    void previous() override;

    /**
     * Serazeni hodnot datasetu podle zadanych klicu.
     * Poradi klicu urcuje jejich prioritu.
     * @param options
     */
    void sort(SortOptions& options);

    /**
     * Vyhledani zaznamu podle zadanych klicu
     * @param options
     */
    void find(SearchOptions& options);

    /**
     * Nalezeni field podle jeho jmena
     * @param name
     * @return
     */
    Field* fieldByName(const std::string& name) override;

    /**
     * Nalezeni field podle jeho indexu
     * @param index
     * @return
     */
    Field* fieldByIndex(int index) override ;

    bool eof() override;

};


#endif //CSV_READER_INMEMORYDATASET_H
