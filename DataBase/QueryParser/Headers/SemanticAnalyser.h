//
// Created by Petr Flajsingr on 2019-02-12.
//

#ifndef PROJECT_SEMANTICANALYSER_H
#define PROJECT_SEMANTICANALYSER_H

#include <QueryCommon.h>
#include <QueryException.h>
#include <string>

namespace DataBase {
class SemanticAnalyser {
 public:
  void setInput(StructuredQuery &structuredQuery);

  void analyse();
 private:
  StructuredQuery structuredQuery;

  enum class SemErrType {
    where, joinMain, joinSecond, group, order, project, having
  };

  std::string getErrorMsg(SemErrType errType, const std::string &value);
};
}

#endif //PROJECT_SEMANTICANALYSER_H
