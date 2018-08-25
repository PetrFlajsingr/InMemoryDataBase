//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef CSV_READER_INMEMORYDATASET_H
#define CSV_READER_INMEMORYDATASET_H


#include "../Interfaces/IDataset.h"


/**
 * Dataset ukladajici data primo v operacni pameti.
 */
class InMemorydataset : public IDataset {
public:
    struct SearchOptions {
        std::vector<unsigned long>  fieldIndexes;
        std::vector<std::string>    searchStrings;

        void addOption(const unsigned long fieldIndex, const std::string &searchString){
            this->fieldIndexes.push_back(fieldIndex);
            this->searchStrings.push_back(searchString);
        }
    };
private:

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

    unsigned long currentRecord = 0;

    bool fieldTypesSet = false;

    bool dataValidityChanged = false;

    std::vector<std::vector<DataContainer*>> data;
    std::vector<bool> dataValidity;

    void loadData();

    void addRecord();

    /**
     * Vytvoreni Fields se jmeny podle nazvu sloupcu.
     * @param columns nazvy sloupcu
     */
    void createFields(std::vector<std::string> columns, std::vector<ValueType> types = {});

    bool setFieldValues(unsigned long index, bool searchForward);

    bool isFieldTypeSet() {
        return !(fields[0].getFieldType() == NONE);
    }

    void setFieldTypes(std::vector<InMemorydataset::DataContainer*> value);
public:
    ~InMemorydataset() override;

    /**
     * Nacteni dat do pameti datasetu.
     * @param provider objekt dodavajici data
     */
    void open() override;

    void setDataProvider(IDataProvider* provider) override ;

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
    void sort(unsigned long fieldIndex, SortOrder sortOrder);

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
    Field * fieldByName(const std::string& name) override;

    /**
     * Nalezeni field podle jeho indexu
     * @param index
     * @return
     */
    Field* fieldByIndex(unsigned long index) override ;

    bool eof() override;

    std::vector<std::string> getFieldNames() override {
        std::vector<std::string> result;
        for(const auto &field : fields) {
            result.push_back(field.fieldName);
        }

        return result;
    }

    void setFieldTypes(std::vector<ValueType> types) override;
};


#endif //CSV_READER_INMEMORYDATASET_H
