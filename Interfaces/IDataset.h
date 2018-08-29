//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef CSV_READER_IDATASET_H
#define CSV_READER_IDATASET_H

#include <string>
#include <vector>
#include <cstring>
#include "IFieldPre.h"
#include "IDataProvider.h"
#include "../Misc/Types.h"

/**
 * Rozhrani pro objekt dodavajici data.
 */
class IDataset {
protected:
    friend class IField; //< kvuli pristupu k IField::setData(...)

    std::vector<IField*> fields; //< Pole umoznujici pristup k datum

    /**
     * Nastaveni dat pole.
     *
     * Zpristupnuje IField::setData potomkum
     * @param field Field, kteremu budou nastavena data
     * @param data Data pro zapis
     */
    void setFieldData(IField *field, void *data);

    /**
     * Nastaveni dat uvnitr datasetu.
     *
     * Volano pri externi zmene dat Field
     * @param data Data k zapisu
     * @param index Index v zaznamu
     * @param type Typ dat
     */
    virtual void setData(void * data, unsigned long index, ValueType type)= 0;
public:
    /**
     * Nacteni dat datasetu z IDataProvider
     */
    virtual void open()= 0;

    /**
     * Nastaveni IDataProvider dodavajici data
     * @param provider
     */
    virtual void setDataProvider(IDataProvider* provider)= 0;

    /**
     * Uzavreni datasetu a znepristupneni jeho dat
     */
    virtual void close()= 0;

    /**
     * Presun na prvni polozku datasetu
     */
    virtual void first()= 0;

    /**
     * Presun na posledni polozku datasetu
     */
    virtual void last()= 0;

    /**
     * Presun na nasledujici polozku datasetu
     */
    virtual void next()= 0;

    /**
     * Presun na predchazejici polozku datasetu
     */
    virtual void previous()= 0;

    /**
     * Kontrola pruchodu vsemi zaznamy.
     * @return false pokud neni dostupny zadny zaznam, jinak true
     */
    virtual bool eof()= 0;

    /**
     * Field podle nazvu sloupce
     * @param name Nazev sloupce/Field
     * @return
     */
    virtual IField * fieldByName(const std::string& name)= 0;

    /**
     * Field podle jeho indexu v zaznamu
     * @param index
     * @return
     */
    virtual IField * fieldByIndex(unsigned long index)= 0;

    /**
     * Nastaveni typu Fields.
     * Vytvori objekty IFields.
     * @param types
     */
    virtual void setFieldTypes(std::vector<ValueType> types)= 0;

    /**
     * Nazvy vsech sloupcu v datasetu
     * @return
     */
    virtual std::vector<std::string> getFieldNames()= 0;

    /**
     * Smazani vsech alokovanych Fields
     */
    virtual ~IDataset();
};

#endif //CSV_READER_IDATASET_H
