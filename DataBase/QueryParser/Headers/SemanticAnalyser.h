//
// Created by Petr Flajsingr on 2019-02-12.
//

#ifndef PROJECT_SEMANTICANALYSER_H
#define PROJECT_SEMANTICANALYSER_H

#include <QueryCommon.h>

namespace DataBase {
class SemanticAnalyser {
 public:
  void setInput(StructuredQuery &structuredQuery);

  StructuredQuery analyse();
 private:
  StructuredQuery structuredQuery;
};
}

#endif //PROJECT_SEMANTICANALYSER_H
