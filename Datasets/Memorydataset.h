//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef CSV_READER_INMEMORYDATASET_H
#define CSV_READER_INMEMORYDATASET_H


#include "../Interfaces/IDataset.h"
#include "../Misc/Types.h"
#include "../Interfaces/IField.h"
#include "FilterStructures.h"


/**
 * Dataset ukladajici data primo v operacni pameti.
 */
class Memorydataset : public IDataset {
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
                delete [] data._string;
            }
        }
    };
    //\

    IDataProvider* dataProvider = nullptr; //< dodavatel dat

    bool isOpen = false;

    unsigned long currentRecord = 0; //< Pocitadlo zaznamu

    bool dataValidityChanged = false; //< Nastaveno pri zmene dat naprikald pomoci find

    std::vector<std::vector<DataContainer*>> data; //< Data uvnitr datasetu
    std::vector<bool> dataValidity; //< Vyrazeni dat nevyhovujici pozadavkum

    /**
     * Nacteni dat do this->data
     */
    void loadData();

    /**
     * Pridani zaznamu do this->data()
     */
    void addRecord();

    /**
     * Vytvoreni Fields se jmeny podle nazvu sloupcu.
     * @param columns nazvy sloupcu
     */
    void createFields(std::vector<std::string> columns, std::vector<ValueType> types);

    /**
     * Nastaveni hodnot this->fields.
     *
     * Vynechava zaznamy s dataValidity == false
     * @param index Index Field
     * @param searchForward Smer vyhledavani validniho zaznamu
     * @return
     */
    bool setFieldValues(unsigned long index, bool searchForward);
protected:
    void setData(void *data, unsigned long index, ValueType type) override;
public:
    ~Memorydataset() override;

    void open() override;

    void setDataProvider(IDataProvider* provider) override;

    void close() override;

    void first() override;

    void last() override;

    void next() override;

    void previous() override;

    /**
     * Serazeni hodnot datasetu podle zadanych klicu.
     * Poradi klicu urcuje jejich prioritu.
     * @param options
     */
    virtual void sort(unsigned long fieldIndex, SortOrder sortOrder);

    /**
     * Vyhledani zaznamu podle zadanych klicu
     * @param options
     */
    virtual void filter(FilterOptions &options);

    IField* fieldByName(const std::string& name) override;

    IField* fieldByIndex(unsigned long index) override ;

    bool eof() override;

    std::vector<std::string> getFieldNames() override {
        std::vector<std::string> result;
        for(const auto &field : fields) {
            result.push_back(field->getFieldName());
        }

        return result;
    }

    void setFieldTypes(std::vector<ValueType> types) override;

    void append() override;

    virtual void appendDataProvider(IDataProvider* provider);
};


#endif //CSV_READER_INMEMORYDATASET_H
