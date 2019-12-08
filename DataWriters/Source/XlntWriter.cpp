//
// Created by Petr Flajsingr on 2019-03-19.
//

#include <Exceptions.h>
#include <XlntWriter.h>

DataWriters::XlntWriter::XlntWriter(std::string_view fileName) : BaseDataWriter() {
  ws = wb.active_sheet();
  destination = fileName;
}

DataWriters::XlntWriter::XlntWriter(std::string_view fileName, CharSet outCharSet) : BaseDataWriter(outCharSet) {
  ws = wb.active_sheet();
  destination = fileName;
}

void DataWriters::XlntWriter::writeHeader(const std::vector<std::string> &header) {
  for (gsl::index i = 0; i < static_cast<gsl::index>(header.size()); ++i) {
    if (convert) {
      ws.cell(i + 1, 1).value(converter->convertBack(header[i]));
    } else {
      ws.cell(i + 1, 1).value(header[i]);
    }
  }
}
void DataWriters::XlntWriter::writeRecord(const std::vector<std::string> &record) {
  ++recordNumber;
  for (gsl::index i = 0; i < static_cast<gsl::index>(record.size()); ++i) {
    if (!cellTypes.empty()) {
      switch (cellTypes[i]) {
      case xlnt::cell_type::date:
      {
        if (record[i].empty()) {
          ws.cell(i + 1, recordNumber).value("není k dispozici");
        } else {
          ws.cell(i + 1, recordNumber).value(doubleToDate(Utilities::stringToDouble(record[i])));
        }
        }
        break;
      case xlnt::cell_type::inline_string:
        if (convert) {
          ws.cell(i + 1, recordNumber).value(converter->convertBack(record[i]));
        } else {
          ws.cell(i + 1, recordNumber).value(record[i]);
        }
        break;
      case xlnt::cell_type::number:
        ws.cell(i + 1, recordNumber).value(Utilities::stringToInt(record[i]));
        break;
      case xlnt::cell_type::shared_string:
        if (convert) {
          ws.cell(i + 1, recordNumber).value(converter->convertBack(record[i]));
        } else {
          ws.cell(i + 1, recordNumber).value(record[i]);
        }
        break;
      default:
        if (convert) {
          ws.cell(i + 1, recordNumber).value(converter->convertBack(record[i]));
        } else {
          ws.cell(i + 1, recordNumber).value(record[i]);
        }
      }
    } else {
      if (convert) {
        ws.cell(i + 1, recordNumber).value(converter->convertBack(record[i]));
      } else {
        ws.cell(i + 1, recordNumber).value(record[i]);
      }
    }
  }
}
DataWriters::XlntWriter::~XlntWriter() { wb.save(destination); }

void DataWriters::XlntWriter::setCellTypes(const std::vector<xlnt::cell_type> &types) { cellTypes = types; }
