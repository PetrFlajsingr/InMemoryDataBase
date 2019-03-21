//
// Created by Petr Flajsingr on 2019-03-19.
//

#ifndef PROJECT_XLNTWRITER_H
#define PROJECT_XLNTWRITER_H

#include <string>
#include "BaseDataWriter.h"
#include <xlnt/xlnt.hpp>

namespace DataWriters {
/**
 * Write xlsx files using xlnt library.
 */
class XlntWriter : public BaseDataWriter {
 public:
  /**
   *
   * @param fileName destination
   */
  explicit XlntWriter(std::string_view fileName);
  XlntWriter(std::string_view fileName, CharSet outCharSet);
  void writeHeader(const std::vector<std::string> &header) override;
  void writeRecord(const std::vector<std::string> &record) override;
  /**
   * Save file
   */
  ~XlntWriter() override;

 private:
  xlnt::workbook wb;
  xlnt::worksheet ws;
  std::string destination;
  gsl::index recordNumber = 1;
};
}  // namespace DataWriters

#endif //PROJECT_XLNTWRITER_H
