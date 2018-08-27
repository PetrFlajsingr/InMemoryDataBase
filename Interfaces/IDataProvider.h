//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef CSV_READER_IDATAPROVIDER_H
#define CSV_READER_IDATAPROVIDER_H


#include <string>

/**
 * Jednoduche rozhrani pro cteni a pohyb v zaznamech
 */
class IDataProvider {
public:
    /**
     * Zaznam rozdeleny na sloupce
     * @return
     */
    virtual std::vector<std::string> getRow()= 0;

    /**
     * Hodnota ve vybranem sloupci
     * @param columnIndex Index sloupce
     * @return
     */
    virtual std::string getColumn(unsigned int columnIndex)= 0;

    /**
     * Pocet sloupcu zaznamu
     * @return
     */
    virtual unsigned long getColumnCount()= 0;

    /**
     * Nazvy sloupcu
     * @return
     */
    virtual std::vector<std::string> getColumnNames() = 0;

    /**
     * Pocet prozatim prectenych zaznamu
     * @return
     */
    virtual unsigned long getCurrentRecordNumber()= 0;

    /**
     * Presun na nasledujici zaznam
     * @return
     */
    virtual bool next()= 0;

    /**
     * Presun na predchozi zaznam
     * @return
     */
    virtual bool previous()= 0;

    /**
     * Presun na prvni zaznam
     */
    virtual void first()= 0;

    /**
     * Presun na posledni zaznam
     */
    virtual void last()= 0;

    /**
     * Kontrola dostupnosti zaznamu
     * @return false pokud neni dostupny dalsi zaznam, jinak true
     */
    virtual bool eof()= 0;

    virtual ~IDataProvider() = default;
};



#endif //CSV_READER_IDATAPROVIDER_H
