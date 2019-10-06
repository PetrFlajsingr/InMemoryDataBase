//
// Created by Petr Flajsingr on 2019-02-12.
//

#ifndef PROJECT_SEMANTICANALYSER_H
#define PROJECT_SEMANTICANALYSER_H

#include <QueryCommon.h>
#include <QueryException.h>
#include <string>

// TODO: kontrola, jestli jsou sloupce v group by, order by v projekci
namespace DataBase {
class SemanticAnalyser {
public:
  void setInput(StructuredQuery &newStructuredQuery);
  StructuredQuery analyse();

private:
  StructuredQuery structuredQuery;

  enum class SemErrType { where, joinMain, joinSecond, group, order, project, having };

  static std::string getErrorMsg(SemErrType errType, const std::string &value);
};
} // namespace DataBase

#endif // PROJECT_SEMANTICANALYSER_H
