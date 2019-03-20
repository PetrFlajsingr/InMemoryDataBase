//
// Created by Petr Flajsingr on 13/11/2018.
//

#ifndef CSV_READER_ARRAYWRITER_H
#define CSV_READER_ARRAYWRITER_H

#include <string>
#include <vector>
#include <BaseDataWriter.h>

namespace DataWriters {
/**
 * Write values to array. For debug/testing purposes.
 */
class ArrayWriter : public BaseDataWriter {
 public:
  void writeHeader(const std::vector<std::string> &header) override;
  void writeRecord(const std::vector<std::string> &record) override;
  /**
   * Print data to stdout.
   */
  void print() const;
  const std::vector<const std::vector<std::string>> &getArray() const;

 private:
  std::vector<const std::vector<std::string>> result;
};

}  // namespace DataWriters

#endif //CSV_READER_ARRAYWRITER_H
