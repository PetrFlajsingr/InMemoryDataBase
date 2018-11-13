//
// Created by Petr Flajsingr on 13/11/2018.
//

#ifndef CSV_READER_BASEDATAWRITER_H
#define CSV_READER_BASEDATAWRITER_H

#include <string>
#include <vector>

class BaseDataWriter {
 public:
  virtual void writeHeader(const std::vector<std::string> &header)=0;

  virtual void writeRecord(const std::vector<std::string> &record)=0;
};

#endif //CSV_READER_BASEDATAWRITER_H
