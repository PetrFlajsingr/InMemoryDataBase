//
// Created by Petr Flajsingr on 2019-02-08.
//

#include <QueryParser.h>
#include <Utilities.h>

void DataBase::QueryLexicalAnalyser::setInput(const std::string &input) {
  this->input = input;
  currentIndex = 0;
}

std::tuple<DataBase::Token,
           std::string,
           bool> DataBase::QueryLexicalAnalyser::getNextToken() {
  auto it = input.begin() + currentIndex;

  auto state = LexState::start;
  std::string value;
  bool isComment = false;
  bool notLast = true;
  Token token;

  while (it != input.end()) {
    if (isComment && *it != '#') {
      isComment = false;
    } else if (*it == '#') {
      isComment = true;
    }
    if (isComment) {
      continue;
    }
    switch (state) {
      case LexState::start:
        if (*it == ' ') {
          state = LexState::start;
          it++;
          continue;
        } else if (isdigit(*it) || *it == '-') {
          state = LexState::num1;
        } else if (isalpha(*it)) {
          state = LexState::id;
        } else if (*it == '\"') {
          state = LexState::string;
        } else if (*it == '=') {
          token = Token::equal;
          value += *it;
          goto emit_token_move_iter;
        } else if (*it == '(') {
          token = Token::leftBracket;
          value += *it;
          goto emit_token_move_iter;
        } else if (*it == ')') {
          token = Token::rightBracket;
          value += *it;
          goto emit_token_move_iter;
        } else if (*it == '.') {
          token = Token::dot;
          value += *it;
          goto emit_token_move_iter;
        } else if (*it == '|') {
          token = Token::pipe;
          value += *it;
          goto emit_token_move_iter;
        } else if (*it == '*') {
          token = Token::asterisk;
          value += *it;
          goto emit_token_move_iter;
        } else if (*it == ';') {
          token = Token::semicolon;
          value += *it;
          goto emit_token_move_iter;
        } else if (*it == ',') {
          token = Token::colon;
          value += *it;
          goto emit_token_move_iter;
        } else if (*it == '<') {
          state = LexState::less;
        } else if (*it == '>') {
          state = LexState::greater;
        } else if (*it == '!') {
          state = LexState::exclam;
        } else {
          throw LexException(getErrorPrint().c_str());
        }
        break;
      case LexState::num1:
        if (isdigit(*it)) {
          state = LexState::num1;
        } else if (*it == '.') {
          state = LexState::numFloat;
        } else {
          token = Token::number;
          goto emit_token;
        }
        break;
      case LexState::numFloat:
        if (isdigit(*it)) {
          state = LexState::numFloat;
        } else {
          token = Token::numberFloat;
          goto emit_token;
        }
        break;
      case LexState::id:
        if (isalnum(*it) || *it == '_' || *it == '-') {
          state = LexState::id;
        } else {
          token = keyWordCheck(Utilities::toLower(value));
          goto emit_token;
        }
        break;
      case LexState::string:
        if (*it == '\"') {
          token = Token::string;
          value += *it;
          goto emit_token_move_iter;
        } else {
          state = LexState::string;
        }
        break;
      case LexState::less:
        if (*it == '=') {
          token = Token::lessEqual;
          value += *it;
          goto emit_token_move_iter;
        } else {
          token = Token::less;
          goto emit_token;
        }
      case LexState::greater:
        if (*it == '=') {
          token = Token::greaterEqual;
          value += *it;
          goto emit_token_move_iter;
        } else {
          token = Token::greater;
          goto emit_token;
        }
      case LexState::exclam:
        if (*it == '=') {
          token = Token::notEqual;
          value += *it;
          goto emit_token_move_iter;
        } else {
          throw LexException(getErrorPrint().c_str());
        }
    }

    value += *it;
    it++;
  }
  emit_token_move_iter:
  it++;
  emit_token:
  if (it == input.end()) {
    notLast = false;
  }

  currentIndex = std::distance(input.begin(), it);
  return std::make_tuple(token, value, notLast);
}

DataBase::Token DataBase::QueryLexicalAnalyser::keyWordCheck(std::string_view str) {
  if (Utilities::compareString(str, "select") == 0) {
    return Token::select;
  } else if (Utilities::compareString(str, "from") == 0) {
    return Token::from;
  } else if (Utilities::compareString(str, "where") == 0) {
    return Token::where;
  } else if (Utilities::compareString(str, "order") == 0) {
    return Token::order;
  } else if (Utilities::compareString(str, "by") == 0) {
    return Token::by;
  } else if (Utilities::compareString(str, "group") == 0) {
    return Token::group;
  } else if (Utilities::compareString(str, "join") == 0) {
    return Token::join;
  } else if (Utilities::compareString(str, "left") == 0) {
    return Token::left;
  } else if (Utilities::compareString(str, "right") == 0) {
    return Token::right;
  } else if (Utilities::compareString(str, "outer") == 0) {
    return Token::outer;
  } else if (Utilities::compareString(str, "having") == 0) {
    return Token::having;
  } else if (Utilities::compareString(str, "sum") == 0) {
    return Token::sum;
  } else if (Utilities::compareString(str, "avg") == 0) {
    return Token::avg;
  } else if (Utilities::compareString(str, "min") == 0) {
    return Token::min;
  } else if (Utilities::compareString(str, "max") == 0) {
    return Token::max;
  } else if (Utilities::compareString(str, "count") == 0) {
    return Token::count;
  } else if (Utilities::compareString(str, "asc") == 0) {
    return Token::asc;
  } else if (Utilities::compareString(str, "desc") == 0) {
    return Token::desc;
  } else if (Utilities::compareString(str, "or") == 0) {
    return Token::orLogic;
  } else if (Utilities::compareString(str, "and") == 0) {
    return Token::andLogic;
  } else if (Utilities::compareString(str, "on") == 0) {
    return Token::on;
  } else {
    return Token::id;
  }
}

std::string DataBase::QueryLexicalAnalyser::getErrorPrint() {
  return "Lexical error, character#" + std::to_string(currentIndex + 1) + "\n"
      + input.substr(0, currentIndex + 1) + "^here^->"
      + input.substr(currentIndex + 1);
}
