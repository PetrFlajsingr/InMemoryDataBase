//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef CSV_READER_CSVREADER_H
#define CSV_READER_CSVREADER_H


#include <fstream>
#include <vector>
#include "IDataProvider.h"
#include "Exceptions.h"

// TODO: rozšířit o nastavení delimiteru
/**
 * Trida implementujici metody IReader.
 * Slouzi ke cteni CSV formatu sekvencne ze souboru.
 */
class CsvReader : public IDataProvider {
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

    /**
     * Zpracovani zaznamu pred jeho zpristupnenim
     */
    void parseRecord();
public:
    /**
     * Vytvori ctecku nad zadanym souborem.
     *
     * Pokud se soubor nepodari otevrit je vyhozena IOException.
     * @param filePath Cesta k souboru
     */
    explicit CsvReader(std::string filePath);

    /**
     * Uzavreni souboru pri dealokaci objektu.
     */
    ~CsvReader() override;

    /**
     * Vraci zaznam souboru, ktery je na rade.
     * @return rozdeleny zaznamu podle CSV delimiter.
     */
    inline std::vector<std::string> getRow() override {
        return std::vector<std::string>(currentRecord);
    }

    inline std::string getColumn(unsigned int columnIndex) override {
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

    void first() override;

    void last() override {
        throw NotImplementedException();
    }

    unsigned long getCurrentRecordNumber() override {
        return this->currentRecordNumber;
    }

    inline std::vector<std::string> getColumnNames() override {
        return std::vector<std::string>(header);
    }

    inline unsigned long getColumnCount() override {
        return header.size();
    }

    bool eof() override;
};


#endif //CSV_READER_CSVREADER_H
