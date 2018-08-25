//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef CSV_READER_IREADER_H
#define CSV_READER_IREADER_H


#include <string>

/**
 * Jednoduche rozhrani pro cteni a pohyb v zaznamech
 */
class IReader {
public:
    virtual std::vector<std::string> getRow()= 0;

    virtual std::string getColumn(unsigned int columnIndex)= 0;

    virtual unsigned long getColumnCount()= 0;

    virtual std::vector<std::string> getColumnNames() = 0;

    virtual unsigned long getCurrentRecordNumber()= 0;

    virtual bool next()= 0;

    virtual bool previous()= 0;

    virtual void first()= 0;

    virtual void last()= 0;
};


#endif //CSV_READER_IREADER_H
