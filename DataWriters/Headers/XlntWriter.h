//
// Created by Petr Flajsingr on 2019-03-19.
//

#ifndef PROJECT_XLNTWRITER_H
#define PROJECT_XLNTWRITER_H

#include "BaseDataWriter.h"
#include <xlnt/xlnt.hpp>

namespace DataWriters {
class XlntWriter : public BaseDataWriter {
 public:
  explicit XlntWriter(std::string_view fileName);
  XlntWriter(std::string_view fileName, CharSet outCharSet);
  void writeHeader(const std::vector<std::string> &header) override;
  void writeRecord(const std::vector<std::string> &record) override;
  virtual ~XlntWriter();

 private:
  xlnt::workbook wb;
  xlnt::worksheet ws;

  std::string destination;

  gsl::index recordNumber = 1;
};
}

#endif //PROJECT_XLNTWRITER_H
