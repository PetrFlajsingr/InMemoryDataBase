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
#include <tuple>

namespace DataBase {
/**
 * Analyse and tokenize query inputs.
 */
class LexicalAnalyser {
 public:
  /**
   *
   * @param input string to tokenize
   */
  void setInput(const std::string &input);
  /**
   * Parse another token
   * @return parsed token, string form of the token, true if this is not last token, false otherwise
   */
  std::tuple<Token, std::string, bool> getNextToken();
  /**
   * Tokenize the entire input
   * @return same as getNextToken() @see LexicalAnalyser::getNextToken()
   */
  std::vector<std::tuple<Token, std::string, bool>> getAllTokens();

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
