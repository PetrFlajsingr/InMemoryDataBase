//
// Created by Petr Flajsingr on 2019-02-08.
//

#ifndef PROJECT_QUERYPARSER_H
#define PROJECT_QUERYPARSER_H

#include <exception>
#include <Exceptions.h>
#include <vector>
#include <string>
#include <QueryCommon.h>
#include <LexicalAnalyser.h>
#include <SyntaxAnalyser.h>

namespace DataBase {
class SemanticAnalyser {
 public:
  void setInput(StructuredQuery &structuredQuery);

  StructuredQuery analyse();
 private:
  StructuredQuery structuredQuery;
};

class QueryAnalyser {
 public:
  StructuredQuery analyseQuery(std::string_view);

 private:
  LexicalAnalyser lexicalAnalyser;
  SyntaxAnalyser syntaxAnalyser;
  SemanticAnalyser semanticAnalyser;
};
}  // namespace DataBase

#endif //PROJECT_QUERYPARSER_H
