//
// Created by Petr Flajsingr on 2019-02-12.
//

#include <SyntaxAnalyser.h>

void DataBase::SyntaxAnalyser::setInput(const std::vector<std::tuple<Token,
                                                                     std::string,
                                                                     bool>> &tokens) {
  this->tokens = tokens;
}

DataBase::StructuredQuery DataBase::SyntaxAnalyser::analyse() {
  StructuredQuery result;

  auto state = SynState::start;

  for (const auto &it : tokens) {
    auto token = std::get<0>(it);
    if (token == Token::space) {
      continue;
    }
    switch (state) {
      case SynState::start:
        if (token == Token::select) {
          state = SynState::selectList;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::missing,
                                            {Token::select},
                                            it));
        }
        break;
      case SynState::selectList:
        if (token == Token::id || token == Token::asterisk) {
          state = SynState::selectItem;
        } else if (std::find(agrFunc.begin(), agrFunc.end(), token)
            != agrFunc.end()) {
          state = SynState::selectAgrItem;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::selectAgrItem:
        if (token == Token::leftBracket) {
          state = SynState::selectAgrItemId;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::leftBracket},
                                            it));
        }
        break;
      case SynState::selectAgrItemId:
        if (token == Token::id) {
          state = SynState::selectAgrItemIdDot;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::selectAgrItemIdDot:
        if (token == Token::dot) {
          state = SynState::selectAgrItemId2;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::dot},
                                            it));
        }
        break;
      case SynState::selectAgrItemId2:
        if (token == Token::id) {
          state = SynState::selectAgrItemEnd;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::selectAgrItemEnd:
        if (token == Token::rightBracket) {
          state = SynState::selectItemDivide;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::rightBracket},
                                            it));
        }
        break;
      case SynState::selectItem:
        if (token == Token::dot) {
          state = SynState::selectItemEnd;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::dot},
                                            it));
        }
        break;
      case SynState::selectItemEnd:
        if (token == Token::id) {
          state = SynState::selectItemDivide;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::selectItemDivide:
        if (token == Token::colon) {
          state = SynState::selectList;
        } else if (token == Token::from) {
          state = SynState::from;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::from, Token::colon},
                                            it));
        }
        break;
      case SynState::from:
        if (token == Token::id) {
          state = SynState::joinList;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::joinList:
        if (token == Token::join) {
          state = SynState::joinItem;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::join},
                                            it));
        }
        break;
      case SynState::joinItem:
        if (token == Token::id) {
          state = SynState::joinOn;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::joinOn:
        if (token == Token::on) {
          state = SynState::joinCond1st;
        } else if (token == Token::semicolon) {
          state = SynState::end;
        } else if (token == Token::where) {
          state = SynState::where;
        } else if (token == Token::group) {
          state = SynState::group;
        } else if (token == Token::order) {
          state = SynState::order;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::on, Token::semicolon,
                                             Token::where, Token::group,
                                             Token::order},
                                            it));
        }
        break;
      case SynState::joinCond1st:
        if (token == Token::id) {
          state = SynState::joinCond1stDot;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::joinCond1stDot:
        if (token == Token::dot) {
          state = SynState::joinCond1stId;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::dot},
                                            it));
        }
        break;
      case SynState::joinCond1stId:
        if (token == Token::id) {
          state = SynState::joinCondEq;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::joinCondEq:
        if (token == Token::equal) {
          state = SynState::joinCond2nd;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::joinCond2nd:
        if (token == Token::id) {
          state = SynState::joinCond2ndDot;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::joinCond2ndDot:
        if (token == Token::dot) {
          state = SynState::joinCond2ndId;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::dot},
                                            it));
        }
        break;
      case SynState::joinCond2ndId:
        if (token == Token::id) {
          state = SynState::joinNext;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::joinNext:
        if (token == Token::colon) {
          state = SynState::joinItem;
        } else if (token == Token::where) {
          state = SynState::where;
        } else if (token == Token::group) {
          state = SynState::group;
        } else if (token == Token::order) {
          state = SynState::order;
        } else if (token == Token::semicolon) {
          state = SynState::end;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::colon, Token::where,
                                             Token::group, Token::order,
                                             Token::semicolon},
                                            it));
        }
        break;
      case SynState::where:
        if (token == Token::id) {
          state = SynState::whereItemDot;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::whereItemDot:
        if (token == Token::dot) {
          state = SynState::whereItem2;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::dot},
                                            it));
        }
        break;
      case SynState::whereItem2:
        if (token == Token::id) {
          state = SynState::whereItemCmp;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::whereItemCmp:
        if (std::find(cmpFunc.begin(), cmpFunc.end(), token) != cmpFunc.end()) {
          state = SynState::whereItem2nd;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            cmpFunc,
                                            it));
        }
        break;
      case SynState::whereItem2nd:
        if (std::find(constants.begin(), constants.end(), token)
            != constants.end()) {
          state = SynState::whereCnst;
        } else if (token == Token::id) {
          state = SynState::whereItem2ndId;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            constants,
                                            it));
        }
        break;
      case SynState::whereCnst:
        if (token == Token::orLogic || token == Token::andLogic) {
          state = SynState::where;
        } else if (token == Token::pipe) {
          state = SynState::whereItem2nd;
        } else if (token == Token::group) {
          state = SynState::group;
        } else if (token == Token::order) {
          state = SynState::order;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::pipe, Token::orLogic,
                                             Token::andLogic},
                                            it));
        }
        break;
      case SynState::whereItem2ndId:
        if (token == Token::id) {
          state = SynState::whereItem2ndDot;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::whereItem2ndDot:
        if (token == Token::dot) {
          state = SynState::whereItem2ndId2;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::dot},
                                            it));
        }
        break;
      case SynState::whereItem2ndId2:
        if (token == Token::id) {
          state = SynState::whereItemId;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::whereItemId:
        if (token == Token::orLogic || token == Token::andLogic) {
          state = SynState::where;
        } else if (token == Token::pipe) {
          state = SynState::whereItem2nd;
        } else if (token == Token::group) {
          state = SynState::group;
        } else if (token == Token::order) {
          state = SynState::order;
        } else if (token == Token::semicolon) {
          state = SynState::end;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::pipe, Token::orLogic,
                                             Token::andLogic},
                                            it));
        }
        break;
      case SynState::group:
        if (token == Token::by) {
          state = SynState::groupBy;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::by},
                                            it));
        }
        break;
      case SynState::groupBy:
        if (token == Token::id) {
          state = SynState::groupIdDot;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::groupIdDot:
        if (token == Token::dot) {
          state = SynState::groupId2;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::dot},
                                            it));
        }
        break;
      case SynState::groupId2:
        if (token == Token::id) {
          state = SynState::groupIdNext;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::groupIdNext:
        if (token == Token::colon) {
          state = SynState::groupBy;
        } else if (token == Token::having) {
          state = SynState::havingItem;
        } else if (token == Token::order) {
          state = SynState::order;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::missing,
                                            {Token::having, Token::colon,
                                             Token::order},
                                            it));
        }
        break;
      case SynState::havingItem:
        if (token == Token::id) {
          state = SynState::havingItemDot;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::havingItemDot:
        if (token == Token::dot) {
          state = SynState::havingItem2;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::dot},
                                            it));
        }
        break;
      case SynState::havingItem2:
        if (token == Token::id) {
          state = SynState::havingItemCmp;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::havingItemCmp:
        if (std::find(cmpFunc.begin(), cmpFunc.end(), token) != cmpFunc.end()) {
          state = SynState::whereItem2nd;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            cmpFunc,
                                            it));
        }
        break;
      case SynState::havingItem2nd:
        if (std::find(constants.begin(), constants.end(), token)
            != constants.end()) {
          state = SynState::havingCnst;
        } else if (token == Token::id) {
          state = SynState::havingItem2ndId;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            constants,
                                            it));
        }
        break;
      case SynState::havingCnst:
        if (token == Token::orLogic || token == Token::andLogic) {
          state = SynState::havingItem;
        } else if (token == Token::pipe) {
          state = SynState::havingItem2nd;
        } else if (token == Token::order) {
          state = SynState::order;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::pipe, Token::orLogic,
                                             Token::andLogic},
                                            it));
        }
        break;
      case SynState::havingItem2ndId:
        if (token == Token::id) {
          state = SynState::havingItem2ndDot;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::havingItem2ndDot:
        if (token == Token::dot) {
          state = SynState::havingItem2ndId2;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::dot},
                                            it));
        }
        break;
      case SynState::havingItem2ndId2:
        if (token == Token::id) {
          state = SynState::havingItemId;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::havingItemId:
        if (token == Token::orLogic || token == Token::andLogic) {
          state = SynState::havingItem;
        } else if (token == Token::pipe) {
          state = SynState::havingItem2nd;
        } else if (token == Token::order) {
          state = SynState::order;
        } else if (token == Token::semicolon) {
          state = SynState::end;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::order, Token::pipe,
                                             Token::orLogic, Token::andLogic,
                                             Token::semicolon},
                                            it));
        }
        break;
      case SynState::order:
        if (token == Token::by) {
          state = SynState::orderBy;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::by},
                                            it));
        }
        break;
      case SynState::orderBy:
        if (token == Token::id) {
          state = SynState::orderId;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::orderId:
        if (token == Token::dot) {
          state = SynState::orderIdDot;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::dot},
                                            it));
        }
        break;
      case SynState::orderIdDot:
        if (token == Token::id) {
          state = SynState::orderIdDir;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::id},
                                            it));
        }
        break;
      case SynState::orderIdDir:
        if (token == Token::asc || token == Token::desc) {
          state = SynState::orderItemDone;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::asc, Token::desc},
                                            it));
        }
        break;
      case SynState::orderItemDone:
        if (token == Token::colon) {
          state = SynState::orderBy;
        } else if (token == Token::semicolon) {
          state = SynState::end;
        } else {
          throw SyntaxException(getErrorMsg(SynErrType::wrong,
                                            {Token::semicolon, Token::colon},
                                            it));
        }
        break;
      case SynState::end:throw SyntaxException("Unexpected token after \";\".");
    }
  }
  if (state != SynState::end) {
    throw SyntaxException("Command not properly ended.");
  }
  return result;
}

std::string DataBase::SyntaxAnalyser::getErrorMsg(DataBase::SyntaxAnalyser::SynErrType errType,
                                                  const std::vector<
                                                      DataBase::Token> &tokens,
                                                  std::tuple<Token,
                                                             std::string,
                                                             bool> token2) {
  const std::string header = "Syntax error: ";
  std::string tok1Info = tokenToString(tokens[0]);
  for (int i = 1; i < tokens.size(); ++i) {
    tok1Info += " or " + tokenToString(tokens[i]);
  }
  auto tok2Info =
      tokenToString(std::get<0>(token2)) + " \"" + std::get<1>(token2) + "\"";
  switch (errType) {
    case SynErrType::missing:
      return header + "Missing token: " + tok1Info + ", replaced by: "
          + tok2Info;
    case SynErrType::wrong:
      return header + "Unexpected token: " + tok2Info + ", expected: "
          + tok1Info;
  }
}
