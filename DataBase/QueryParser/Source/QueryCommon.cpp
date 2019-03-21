//
// Created by Petr Flajsingr on 2019-02-12.
//

#include <QueryCommon.h>
#include <Exceptions.h>

std::string DataBase::tokenToString(DataBase::Token token) {
  switch (token) {
    case Token::id: return "id";
    case Token::number: return "number";
    case Token::numberFloat: return "numberFloat";
    case Token::string: return "string";
    case Token::equal: return "equal";
    case Token::notEqual: return "notEqual";
    case Token::greater: return "greater";
    case Token::greaterEqual: return "greaterEqual";
    case Token::less: return "less";
    case Token::lessEqual: return "lessEqual";
    case Token::leftBracket: return "leftBracket";
    case Token::rightBracket: return "rightBracket";
    case Token::dot: return "dot";
    case Token::colon: return "colon";
    case Token::semicolon: return "semicolon";
    case Token::pipe: return "pipe";
    case Token::space: return "space";
    case Token::asterisk: return "asterisk";
    case Token::select: return "select";
    case Token::group: return "group";
    case Token::from: return "from";
    case Token::where: return "where";
    case Token::order: return "order";
    case Token::by: return "by";
    case Token::join: return "join";
    case Token::left: return "left";
    case Token::right: return "right";
    case Token::outer: return "outer";
    case Token::having: return "having";
    case Token::sum: return "sum";
    case Token::avg: return "avg";
    case Token::min: return "min";
    case Token::max: return "max";
    case Token::count: return "count";
    case Token::asc: return "asc";
    case Token::desc: return "desc";
    case Token::logicOr: return "orLogic";
    case Token::logicAnd: return "andLogic";
    case Token::on: return "on";
    case Token::as: return "as";
    case Token::unknown: return "unknown";
  }
}

DataBase::CondOperator DataBase::tokenToCondOperator(DataBase::Token token) {
  switch (token) {
    case Token::greater: return CondOperator::greater;
    case Token::greaterEqual: return CondOperator::greaterEqual;
    case Token::less: return CondOperator::less;
    case Token::lessEqual: return CondOperator::lessEqual;
    case Token::equal: return CondOperator::equal;
    case Token::notEqual: return CondOperator::notEqual;
    default:throw IllegalStateException("Internal error: DataBase::tokenToCondOperator");
  }
}

DataBase::ConstType DataBase::tokenToConstType(DataBase::Token token) {
  switch (token) {
    case Token::number: return ConstType::integer;
    case Token::numberFloat: return ConstType::floatingPoint;
    case Token::string: return ConstType::string;
    default:throw IllegalStateException("Internal error: DataBase::tokenToConstType");
  }
}

DataBase::LogicOperator DataBase::tokenToLogic(DataBase::Token token) {
  switch (token) {
    case Token::logicOr: return LogicOperator::logicOr;
    case Token::logicAnd: return LogicOperator::logicAnd;
    default:throw IllegalStateException("Internal error: DataBase::tokenToConstType");
  }
}

DataBase::AgrOperator DataBase::tokenToAgrOperation(DataBase::Token token) {
  switch (token) {
    case Token::sum: return AgrOperator::sum;
    case Token::avg: return AgrOperator::avg;
    case Token::min: return AgrOperator::min;
    case Token::max: return AgrOperator::max;
    case Token::count: return AgrOperator::count;
    default:throw IllegalStateException("Internal error: DataBase::tokenToConstType");
  }
}

DataBase::Order DataBase::tokenToOrder(DataBase::Token token) {
  switch (token) {
    case Token::asc: return Order::asc;
    case Token::desc: return Order::desc;
    default:throw IllegalStateException("Internal error: DataBase::tokenToConstType");
  }
}

