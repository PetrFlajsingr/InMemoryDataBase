//
// Created by petr on 3/18/20.
//
#include "LoadingUtils.h"
#include "MemoryDataBase.h"
#include "StdoutWriter.h"
#include <io/print.h>
int main() {
  ExcelDateTime2DateTimeConverter cvt;
  print(cvt.convert(30829));
  MacExcelDateTime2DateTimeConverter cvt2;
  print(cvt2.convert(29367));
}