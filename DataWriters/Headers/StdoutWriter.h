//
// Created by Petr Flajsingr on 2019-02-23.
//

#ifndef PROJECT_STDOUTWRITER_H
#define PROJECT_STDOUTWRITER_H

#include <iostream>
#include "BaseDataWriter.h"

namespace DataWriters {
/**
 * Write data directly to stdout. Mainly for debug/test.
 */
class StdoutWriter : public BaseDataWriter {
 public:
  void writeHeader(const std::vector<std::string> &header) override;
  void writeRecord(const std::vector<std::string> &record) override;
};
}  // namespace DataWriters

#endif //PROJECT_STDOUTWRITER_H
