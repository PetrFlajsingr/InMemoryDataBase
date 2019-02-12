//
// Created by Petr Flajsingr on 2019-02-12.
//

#ifndef PROJECT_SYNTAXANALYSER_H
#define PROJECT_SYNTAXANALYSER_H

#include <string>
#include <vector>
#include <QueryCommon.h>
#include <QueryException.h>

namespace DataBase {
// TODO: agre functions
class SyntaxAnalyser {
 public:
  void setInput(const std::vector<std::tuple<Token,
                                             std::string,
                                             bool>> &tokens);

  StructuredQuery analyse();
 private:
  enum class SynState {
    start,
    selectList,
    selectItem,
    selectAgrItem,
    selectAgrItemId,
    selectAgrItemIdDot,
    selectAgrItemId2,
    selectAgrItemEnd,
    selectItemEnd,
    selectItemDivide,
    from,
    joinLeft,
    joinRight,
    joinOuter,
    joinList,
    joinItem,
    joinOn,
    joinCond1st,
    joinCond1stDot,
    joinCond1stId,
    joinCondEq,
    joinCond2nd,
    joinCond2ndDot,
    joinCond2ndId,
    where,
    group,
    order,
    whereItemDot,
    whereItem2,
    whereItemCmp,
    whereItem2nd,
    whereCnst,
    whereItem2ndId,
    whereItem2ndDot,
    whereItemId,
    groupBy,
    groupIdDot,
    groupId2,
    groupIdNext,
    havingItem,
    havingLeftBracket,
    havingItemDot,
    havingItemIdIn,
    havingRightBracket,
    havingItem2,
    havingItemCmp,
    havingItem2nd,
    havingCnst,
    havingItem2ndId,
    havingItem2ndDot,
    havingItem2ndId2,
    havingItemId,
    orderBy,
    orderId,
    orderIdDot,
    orderIdDir,
    orderItemDone,
    end
  };

  enum class SynErrType {
    missing, wrong
  };

  gsl::index currentToken = 0;

  const std::vector<Token> cmpFunc
      {Token::less, Token::lessEqual, Token::greater, Token::greaterEqual,
       Token::equal, Token::notEqual};
  const std::vector<Token>
      constants{Token::number, Token::numberFloat, Token::string};
  const std::vector<Token>
      agrFunc{Token::sum, Token::avg, Token::min, Token::max, Token::count};

  std::vector<std::tuple<Token, std::string, bool>> tokens;

  std::string getErrorMsg(DataBase::SyntaxAnalyser::SynErrType errType,
                          const std::vector<DataBase::Token> &tokens,
                          std::tuple<Token, std::string, bool> token2);
};
}

#endif //PROJECT_SYNTAXANALYSER_H
