//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef CSV_READER_CSVREADER_H
#define CSV_READER_CSVREADER_H


#include <fstream>
#include <vector>
#include "IReader.h"
#include "Exceptions.h"

/**
 * Trida implementujici metody IReader.
 * Slouzi ke cteni CSV formatu sekvencne ze souboru.
 */
class CsvReader : public IReader {
private:
    const unsigned int BUFFER_SIZE = 1024; //< velikost bufferu pro cteni radku souboru

    const char DELIMITER = ';'; //< rozdelovac CSV dat

    std::ifstream file; //< vstupni soubor

    std::vector<std::string> header; //< nazvy sloupcu zaznamu

    std::vector<std::string> currentRecord; //< momentalni zaznam, rozdeleny

    unsigned long currentRecordNumber = 0; //< cislo momentalniho zaznamu

    /**
     * Precteni hlavicky CSV souboru a zapsani do this->header
     */
    void readHeader();

    void parseRecord();
public:
    /**
     * Vytvoří ctecku nad zadaným souborem.
     *
     * Pokud se soubor nepodaří otevřít je vyhozena IOException.
     * @param filePath cesta k souboru
     */
    explicit CsvReader(std::string filePath);

    /**
     * Uzavření souboru při dealokaci objektu.
     */
    ~CsvReader();

    /**
     * Vraci zaznam souboru, ktery je na rade.
     * @return rozdeleny zaznamu podle CSV delimiter.
     */
    std::vector<std::string> getRow() override {
        return std::vector<std::string>(currentRecord);
    }

    /**
     * Nazev sloupce podle indexu.
     * @param columnIndex index sloupce
     * @return Nazev sloupce
     */
    std::string getColumn(unsigned int columnIndex) override {
        return this->header.at(columnIndex);
    }

    /**
     * Posun na dalsi zaznam.
     * @return false pokud byl přečten celý soubor (EOF), jinak true
     */
    bool next() override;

    /**
     * Neimplementovana metoda - pouze sekvencni cteni
     * @return
     */
    bool previous() override {
        throw NotImplementedException();
    }

    /**
     * Presun na prvni zaznam.
     */
    void first() override;

    /**
     * Neimplementovana metoda - zbytecne pro CSV
     */
    void last() override {
        throw NotImplementedException();
    }

    unsigned long getCurrentRecordNumber() override {
        return this->currentRecordNumber;
    }

    /**
     * Nazvy vsech sloupcu
     * @return nazvy vsech sloupcu jako vector
     */
    std::vector<std::string> getColumnNames() override {
        return std::vector<std::string>(header);
    }

    unsigned long getColumnCount() override {
        return header.size();
    }
};


#endif //CSV_READER_CSVREADER_H
