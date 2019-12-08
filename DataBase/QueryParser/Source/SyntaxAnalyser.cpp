//
// Created by Petr Flajsingr on 2019-02-12.
//

#include <SyntaxAnalyser.h>

void DataBase::SyntaxAnalyser::setInput(const std::vector<std::tuple<Token, std::string, bool>> &inputTokens) {
  this->tokens = inputTokens;
}

DataBase::StructuredQuery DataBase::SyntaxAnalyser::analyse() {
  currentToken = 0;
  StructuredQuery result;

  auto state = SynState::start;

  ProjectItem projectItem;
  WhereItem whereItem;
  JoinItem joinItem;
  AgreItem agrItem;
  HavingItem havingItem;
  OrderItem orderItem;

  FieldId fieldId;

  bool selectTrueAgrFalse = true;
  for (const auto &it : tokens) {
    auto token = std::get<0>(it);
    if (token == Token::space) {
      currentToken++;
      continue;
    }
    switch (state) {
    case SynState::start:
      if (token == Token::select) {
        state = SynState::selectList;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::missing, {Token::select}, it));
      }
      break;
    case SynState::selectList:
      if (token == Token::id) {
        state = SynState::selectItem;
        projectItem.table = std::get<1>(it);
        projectItem.alias = "";
      } else if (std::find(agrFunc.begin(), agrFunc.end(), token) != agrFunc.end()) {
        state = SynState::selectAgrItem;
        agrItem.op = tokenToAgrOperation(token);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::selectAgrItem:
      if (token == Token::leftBracket) {
        state = SynState::selectAgrItemId;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::leftBracket}, it));
      }
      break;
    case SynState::selectAgrItemId:
      if (token == Token::id) {
        state = SynState::selectAgrItemIdDot;
        projectItem.table = std::get<1>(it);
        agrItem.field.table = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::selectAgrItemIdDot:
      if (token == Token::dot) {
        state = SynState::selectAgrItemId2;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::dot}, it));
      }
      break;
    case SynState::selectAgrItemId2:
      if (token == Token::id) {
        state = SynState::selectAgrItemEnd;
        projectItem.column = std::get<1>(it);
        agrItem.field.column = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::selectAgrItemEnd:
      if (token == Token::rightBracket) {
        state = SynState::selectItemDivide;
        result.project.data.emplace_back(projectItem);
        result.agr.data.emplace_back(agrItem);
        selectTrueAgrFalse = false;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::rightBracket}, it));
      }
      break;
    case SynState::selectItem:
      if (token == Token::dot) {
        state = SynState::selectItemEnd;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::dot}, it));
      }
      break;
    case SynState::selectItemEnd:
      if (token == Token::id || token == Token::asterisk) {
        state = SynState::selectItemDivide;
        projectItem.column = std::get<1>(it);
        result.project.data.emplace_back(projectItem);
        selectTrueAgrFalse = true;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id, Token::asterisk}, it));
      }
      break;
    case SynState::selectItemDivide:
      if (token == Token::as) {
        state = SynState::selectAs;
      } else if (token == Token::colon) {
        state = SynState::selectList;
      } else if (token == Token::from) {
        state = SynState::from;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::as, Token::from, Token::colon}, it));
      }
      break;
    case SynState::selectAs:
      if (token == Token::id) {
        state = SynState::selectItemDivide;
        if (selectTrueAgrFalse) {
          result.project.data.back().alias = std::get<1>(it);
        } else {
          result.agr.data.back().field.alias = std::get<1>(it);
        }
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::from:
      if (token == Token::id) {
        state = SynState::joinList;
        result.mainTable = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::joinList:
      if (token == Token::join) {
        state = SynState::joinItem;
        joinItem.type = JoinType::innerJoin;
      } else if (token == Token::left) {
        state = SynState::joinLeft;
        joinItem.type = JoinType::leftJoin;
      } else if (token == Token::right) {
        state = SynState::joinRight;
        joinItem.type = JoinType::rightJoin;
      } else if (token == Token::outer) {
        state = SynState::joinOuter;
        joinItem.type = JoinType::outerJoin;
      } else if (token == Token::where) {
        state = SynState::where;
      } else if (token == Token::group) {
        state = SynState::group;
      } else if (token == Token::order) {
        state = SynState::order;
      } else if (token == Token::semicolon) {
        state = SynState::end;
      } else {
        throw SyntaxException(getErrorMsg(
            SynErrType::wrong, {Token::join, Token::where, Token::group, Token::order, Token::semicolon}, it));
      }
      break;
    case SynState::joinLeft:
      if (token == Token::join) {
        state = SynState::joinItem;
      } else {
        throw SyntaxException(getErrorMsg(
            SynErrType::wrong, {Token::join, Token::where, Token::group, Token::order, Token::semicolon}, it));
      }
      break;
    case SynState::joinRight:
      if (token == Token::join) {
        state = SynState::joinItem;
      } else {
        throw SyntaxException(getErrorMsg(
            SynErrType::wrong, {Token::join, Token::where, Token::group, Token::order, Token::semicolon}, it));
      }
      break;
    case SynState::joinOuter:
      if (token == Token::join) {
        state = SynState::joinItem;
      } else {
        throw SyntaxException(getErrorMsg(
            SynErrType::wrong, {Token::join, Token::where, Token::group, Token::order, Token::semicolon}, it));
      }
      break;
    case SynState::joinItem:
      if (token == Token::id) {
        state = SynState::joinOn;
        joinItem.joinedTable = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
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
                                          {Token::on, Token::semicolon, Token::where, Token::group, Token::order}, it));
      }
      break;
    case SynState::joinCond1st:
      if (token == Token::id) {
        state = SynState::joinCond1stDot;
        joinItem.firstField.table = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::joinCond1stDot:
      if (token == Token::dot) {
        state = SynState::joinCond1stId;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::dot}, it));
      }
      break;
    case SynState::joinCond1stId:
      if (token == Token::id) {
        state = SynState::joinCondEq;
        joinItem.firstField.column = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::joinCondEq:
      if (token == Token::equal) {
        state = SynState::joinCond2nd;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::equal}, it));
      }
      break;
    case SynState::joinCond2nd:
      if (token == Token::id) {
        state = SynState::joinCond2ndDot;
        joinItem.secondField.table = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::joinCond2ndDot:
      if (token == Token::dot) {
        state = SynState::joinCond2ndId;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::dot}, it));
      }
      break;
    case SynState::joinCond2ndId:
      if (token == Token::id) {
        state = SynState::joinList;
        joinItem.secondField.column = std::get<1>(it);
        result.joins.data.emplace_back(joinItem);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::where:
      if (token == Token::id) {
        state = SynState::whereItemDot;
        whereItem.values.clear();
        whereItem.constValues.clear();
        whereItem.field.table = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::whereItemDot:
      if (token == Token::dot) {
        state = SynState::whereItem2;
      } else if (std::find(cmpFunc.begin(), cmpFunc.end(), token) != cmpFunc.end()) {
        state = SynState::whereItem2nd;
        whereItem.condOperator = tokenToCondOperator(token);
        whereItem.field.alias = whereItem.field.table;
        whereItem.field.table = "";
        whereItem.field.column = "";
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::dot}, it));
      }
      break;
    case SynState::whereItem2:
      if (token == Token::id) {
        state = SynState::whereItemCmp;
        whereItem.field.column = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::whereItemCmp:
      if (std::find(cmpFunc.begin(), cmpFunc.end(), token) != cmpFunc.end()) {
        state = SynState::whereItem2nd;
        whereItem.condOperator = tokenToCondOperator(token);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, cmpFunc, it));
      }
      break;
    case SynState::whereItem2nd:
      if (std::find(constants.begin(), constants.end(), token) != constants.end()) {
        state = SynState::whereCnst;
        whereItem.constValues.emplace_back(std::make_pair(tokenToConstType(token), std::get<1>(it)));
      } else if (token == Token::id) {
        state = SynState::whereItem2ndId;
        fieldId.table = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, constants, it));
      }
      break;
    case SynState::whereCnst:
      if (token == Token::logicOr || token == Token::logicAnd) {
        state = SynState::where;
        result.where.data.emplace_back(whereItem, tokenToLogic(token));
      } else if (token == Token::pipe) {
        state = SynState::whereItem2nd;
      } else if (token == Token::group) {
        state = SynState::group;
        result.where.data.emplace_back(whereItem, LogicOperator::none);
      } else if (token == Token::order) {
        state = SynState::order;
        result.where.data.emplace_back(whereItem, LogicOperator::none);
      } else if (token == Token::semicolon) {
        state = SynState::end;
        result.where.data.emplace_back(whereItem, LogicOperator::none);
      } else {
        throw SyntaxException(
            getErrorMsg(SynErrType::wrong, {Token::pipe, Token::logicOr, Token::logicAnd, Token::group}, it));
      }
      break;
    case SynState::whereItem2ndId:
      if (token == Token::dot) {
        state = SynState::whereItem2ndDot;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::dot}, it));
      }
      break;
    case SynState::whereItem2ndDot:
      if (token == Token::id) {
        state = SynState::whereItemId;
        fieldId.column = std::get<1>(it);
        whereItem.values.emplace_back(fieldId);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::whereItemId:
      if (token == Token::logicOr || token == Token::logicAnd) {
        state = SynState::where;
        result.where.data.push_back(std::make_pair(whereItem, tokenToLogic(token)));
      } else if (token == Token::pipe) {
        state = SynState::whereItem2nd;
      } else if (token == Token::group) {
        state = SynState::group;
        result.where.data.push_back(std::make_pair(whereItem, LogicOperator::none));
      } else if (token == Token::order) {
        state = SynState::order;
        result.where.data.push_back(std::make_pair(whereItem, LogicOperator::none));
      } else if (token == Token::semicolon) {
        state = SynState::end;
        result.where.data.push_back(std::make_pair(whereItem, LogicOperator::none));
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::pipe, Token::logicOr, Token::logicAnd}, it));
      }
      break;
    case SynState::group:
      if (token == Token::by) {
        state = SynState::groupBy;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::by}, it));
      }
      break;
    case SynState::groupBy:
      if (token == Token::id) {
        state = SynState::groupIdDot;
        agrItem.field.table = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::groupIdDot:
      if (token == Token::dot) {
        state = SynState::groupId2;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::dot}, it));
      }
      break;
    case SynState::groupId2:
      if (token == Token::id) {
        state = SynState::groupIdNext;
        agrItem.field.column = std::get<1>(it);
        agrItem.op = AgrOperator::group;
        result.agr.data.emplace_back(agrItem);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::groupIdNext:
      if (token == Token::colon) {
        state = SynState::groupBy;
      } else if (token == Token::having) {
        state = SynState::havingItem;
      } else if (token == Token::order) {
        state = SynState::order;
      } else if (token == Token::semicolon) {
        state = SynState::end;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::missing, {Token::having, Token::colon, Token::order}, it));
      }
      break;
    case SynState::havingItem:
      if (token == Token::id) {
        state = SynState::havingItemCmp;
        havingItem.agreItem.field.alias = std::get<1>(it);
        havingItem.agreItem.field.table = "";
        havingItem.agreItem.field.column = "";
      } else if (std::find(agrFunc.begin(), agrFunc.end(), token) != agrFunc.end()) {
        state = SynState::havingLeftBracket;
        havingItem.agreItem.op = tokenToAgrOperation(token);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, agrFunc, it));
      }
      break;
    case SynState::havingLeftBracket:
      if (token == Token::leftBracket) {
        state = SynState::havingItemIdIn;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::leftBracket}, it));
      }
      break;
    case SynState::havingItemIdIn:
      if (token == Token::id) {
        state = SynState::havingItemDot;
        havingItem.agreItem.field.table = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::havingItemDot:
      if (token == Token::dot) {
        state = SynState::havingItem2;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::dot}, it));
      }
      break;
    case SynState::havingItem2:
      if (token == Token::id) {
        state = SynState::havingRightBracket;
        havingItem.agreItem.field.column = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::havingRightBracket:
      if (token == Token::rightBracket) {
        state = SynState::havingItemCmp;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::rightBracket}, it));
      }
      break;
    case SynState::havingItemCmp:
      if (std::find(cmpFunc.begin(), cmpFunc.end(), token) != cmpFunc.end()) {
        state = SynState::havingItem2nd;
        havingItem.condOperator = tokenToCondOperator(token);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, cmpFunc, it));
      }
      break;
    case SynState::havingItem2nd:
      if (std::find(constants.begin(), constants.end(), token) != constants.end()) {
        state = SynState::havingCnst;
        havingItem.constValues.emplace_back(tokenToConstType(token), std::get<1>(it));
      } else if (token == Token::id) {
        state = SynState::havingItem2ndId;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, constants, it));
      }
      break;
    case SynState::havingCnst:
      if (token == Token::logicOr || token == Token::logicAnd) {
        state = SynState::havingItem;
        result.having.data.emplace_back(havingItem, tokenToLogic(token));
      } else if (token == Token::pipe) {
        state = SynState::havingItem2nd;
      } else if (token == Token::order) {
        state = SynState::order;
        result.having.data.emplace_back(havingItem, LogicOperator::none);
      } else if (token == Token::semicolon) {
        state = SynState::end;
        result.having.data.emplace_back(havingItem, LogicOperator::none);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::pipe, Token::logicOr, Token::logicAnd}, it));
      }
      break;
    case SynState::havingItem2ndId:
      if (token == Token::id) {
        state = SynState::havingItem2ndDot;
        fieldId.table = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::havingItem2ndDot:
      if (token == Token::dot) {
        state = SynState::havingItem2ndId2;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::dot}, it));
      }
      break;
    case SynState::havingItem2ndId2:
      if (token == Token::id) {
        state = SynState::havingItemId;
        fieldId.column = std::get<1>(it);
        havingItem.values.emplace_back(fieldId);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::havingItemId:
      if (token == Token::logicOr || token == Token::logicAnd) {
        state = SynState::havingItem;
        result.having.data.emplace_back(havingItem, tokenToLogic(token));
      } else if (token == Token::pipe) {
        state = SynState::havingItem2nd;
      } else if (token == Token::order) {
        state = SynState::order;
        result.having.data.emplace_back(havingItem, LogicOperator::none);
      } else if (token == Token::semicolon) {
        state = SynState::end;
        result.having.data.emplace_back(havingItem, LogicOperator::none);
      } else {
        throw SyntaxException(getErrorMsg(
            SynErrType::wrong, {Token::order, Token::pipe, Token::logicOr, Token::logicAnd, Token::semicolon}, it));
      }
      break;
    case SynState::order:
      if (token == Token::by) {
        state = SynState::orderBy;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::by}, it));
      }
      break;
    case SynState::orderBy:
      if (token == Token::id) {
        state = SynState::orderId;
        orderItem.field.table = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::orderId:
      if (token == Token::dot) {
        state = SynState::orderIdDot;
      } else if (token == Token::asc || token == Token::desc) {
        orderItem.field.alias = orderItem.field.table;
        orderItem.field.table = "";
        orderItem.field.column = "";
        state = SynState::orderItemDone;
        orderItem.order = tokenToOrder(token);
        result.order.data.emplace_back(orderItem);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::dot, Token::asc, Token::desc}, it));
      }
      break;
    case SynState::orderIdDot:
      if (token == Token::id) {
        state = SynState::orderIdDir;
        orderItem.field.column = std::get<1>(it);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::id}, it));
      }
      break;
    case SynState::orderIdDir:
      if (token == Token::asc || token == Token::desc) {
        state = SynState::orderItemDone;
        orderItem.order = tokenToOrder(token);
        result.order.data.emplace_back(orderItem);
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::asc, Token::desc}, it));
      }
      break;
    case SynState::orderItemDone:
      if (token == Token::colon) {
        state = SynState::orderBy;
      } else if (token == Token::semicolon) {
        state = SynState::end;
      } else {
        throw SyntaxException(getErrorMsg(SynErrType::wrong, {Token::semicolon, Token::colon}, it));
      }
      break;
    case SynState::end:
      throw SyntaxException("Unexpected token after \";\".");
    }
    currentToken++;
  }
  if (state != SynState::end) {
    throw SyntaxException("Messages not properly ended.");
  }
  return result;
}

std::string DataBase::SyntaxAnalyser::getErrorMsg(DataBase::SyntaxAnalyser::SynErrType errType,
                                                  const std::vector<DataBase::Token> &expectedTokens,
                                                  std::tuple<Token, std::string, bool> token2) {
  const std::string header = "Syntax error: ";
  std::string tok1Info = tokenToString(expectedTokens[0]);
  for (int i = 1; i < static_cast<gsl::index>(expectedTokens.size()); ++i) {
    tok1Info += " or " + tokenToString(expectedTokens[i]);
  }
  auto tok2Info = tokenToString(std::get<0>(token2)) + " \"" + std::get<1>(token2) + "\"";

  std::string query;
  gsl::index i = 0;
  gsl::index lenLimit = 130;
  for (const auto &val : this->tokens) {
    if (i == currentToken) {
      query += ">>" + std::get<1>(val) + "<<";
    } else {
      query += std::get<1>(val);
    }
    if (static_cast<gsl::index>(query.size()) > lenLimit) {
      lenLimit += query.size();
      query += "\n";
    }
    ++i;
  }
  switch (errType) {
  case SynErrType::missing:
    return header + "Missing token: " + tok1Info + ", replaced by: " + tok2Info + "\n" + query;
  case SynErrType::wrong:
    return header + "Unexpected token: " + tok2Info + ", expected: " + tok1Info + "\n" + query;
  }
  throw std::runtime_error("SyntaxAnalyser::getErrorMsg()");
}
