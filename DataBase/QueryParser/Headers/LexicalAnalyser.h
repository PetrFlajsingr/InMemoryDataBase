//
// Created by Petr Flajsingr on 2019-02-12.
//

#ifndef PROJECT_LEXICALANALYSER_H
#define PROJECT_LEXICALANALYSER_H

#include <gsl/gsl>
#include <string>
#include <vector>
#include <QueryCommon.h>
#include <QueryException.h>
#include <Utilities.h>

namespace DataBase {
class LexicalAnalyser {
 public:
  void setInput(const std::string &input);

  std::tuple<Token, std::string, bool> getNextToken();

 private:
  std::string input;

  gsl::index currentIndex;

  Token keyWordCheck(std::string_view str);

  std::string getErrorPrint();

  enum class LexState {
    start, negNum, num1, numFloat, id, string, exclam, less, greater
  };
};
}

#endif //PROJECT_LEXICALANALYSER_H
