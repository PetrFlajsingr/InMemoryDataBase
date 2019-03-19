//
// Created by Petr Flajsingr on 2019-03-19.
//

#ifndef PROJECT_XLNTWRITER_H
#define PROJECT_XLNTWRITER_H

#include "BaseDataWriter.h"
namespace DataWriters {
class XlntWriter : public BaseDataWriter {
 public:
  explicit XlntWriter(std::string_view fileName);
  void writeHeader(const std::vector<std::string> &header) override;
  void writeRecord(const std::vector<std::string> &record) override;

 private:

};
}

#endif //PROJECT_XLNTWRITER_H
