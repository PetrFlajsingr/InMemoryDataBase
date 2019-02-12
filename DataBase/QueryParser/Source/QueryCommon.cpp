//
// Created by Petr Flajsingr on 2019-02-12.
//

#include <QueryCommon.h>

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
    case Token::orLogic: return "orLogic";
    case Token::andLogic: return "andLogic";
    case Token::on: return "on";
  }
}

