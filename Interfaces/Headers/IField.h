//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef CSV_READER_FIELDBASE_H
#define CSV_READER_FIELDBASE_H

#include <string>
#include <utility>
#include "Types.h"
#include "IDatasetFwd.h"

/**
 * Interface pro fields datasetu.
 */
class IField{
protected:
    std::string fieldName; //< Nazev reprezentovaneho sloupce

    unsigned long index; //< Index sloupce
public:
    const std::string &getFieldName() const {
        return fieldName;
    }

protected:
    // Kvuli pristupu k metode IDataset::SetData(...)
    friend class IDataset;

    IDataset* dataset; //< Rodicovsky dataset

    /**
     * Nastaveni hodnoty field.
     * @param data pointer na data
     */
    virtual void setValue(void * data)= 0;

    /**
     * Nastaveni dat v datasetu.
     *
     * Tato funkce zpristupnuje setValue potomkum.
     * @param data data pro ulozeni
     * @param type typ dat
     */
    void setDatasetData(void * data, ValueType type);

public:
    /**
     * Nastaveni datasetu, nazvu a indexu field
     * @param fieldName Nazev field
     * @param dataset Rodicovsky dataset
     * @param index Index pole v zaznamu
     */
    explicit IField(std::string fieldName,
                            IDataset* dataset,
                            unsigned long index) : fieldName(std::move(fieldName)),
                                                   dataset(dataset),
                                                   index(index){}

    virtual ~IField() = default;

    /**
     * Typ dat ulozenych ve Field
     * @return
     */
    virtual ValueType getFieldType()= 0;

    /**
     * Nastaveni hodnoty pole pomoci string
     * @param value
     */
    virtual void setAsString(const std::string& value)= 0;

    /**
     * Navrat hodnoty v poli jako string
     * @return
     */
    virtual std::string getAsString()= 0;
};


#endif //CSV_READER_FIELDBASE_H
