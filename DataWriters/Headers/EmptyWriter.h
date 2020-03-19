//
// Created by petr on 3/10/20.
//

#ifndef CSV_READER_EMPTYWRITER_H
#define CSV_READER_EMPTYWRITER_H

#include "BaseDataWriter.h"
namespace DataWriters {
class EmptyWriter : public BaseDataWriter {
public:
  template <typename ...Args>
  explicit EmptyWriter([[maybe_unused]] const Args ...args) : BaseDataWriter() {}
  void writeHeader([[maybe_unused]] const std::vector<std::string> &header) override;
  void writeRecord([[maybe_unused]] const std::vector<std::string> &record) override;
};
}

#endif // CSV_READER_EMPTYWRITER_H
