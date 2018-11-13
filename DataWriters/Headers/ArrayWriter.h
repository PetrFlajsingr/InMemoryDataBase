//
// Created by Petr Flajsingr on 13/11/2018.
//

#ifndef CSV_READER_ARRAYWRITER_H
#define CSV_READER_ARRAYWRITER_H

#include <BaseDataWriter.h>
class ArrayWriter : public BaseDataWriter {
 public:
  std::vector<std::vector<std::string>> *result;

  void writeHeader(const std::vector<std::string> &header) override;
  void writeRecord(const std::vector<std::string> &record) override;

  void print();
};

#endif //CSV_READER_ARRAYWRITER_H
