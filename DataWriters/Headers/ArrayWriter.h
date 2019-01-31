//
// Created by Petr Flajsingr on 13/11/2018.
//

#ifndef CSV_READER_ARRAYWRITER_H
#define CSV_READER_ARRAYWRITER_H

#include <string>
#include <vector>
#include <BaseDataWriter.h>

namespace DataWriters {

class ArrayWriter : public BaseDataWriter {
 public:
  std::vector<const std::vector<std::string>> result;

  void writeHeader(const std::vector<std::string> &header) override;

  void writeRecord(const std::vector<std::string> &record) override;

  /**
   * Vytiskne obsah zapsanych dat na stdout.
   */
  void print() const;

  const std::vector<const std::vector<std::string>> &getArray() const;
};

}  // namespace DataWriters

#endif //CSV_READER_ARRAYWRITER_H
