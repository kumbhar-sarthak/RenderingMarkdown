#include "bold_rule.h"
#include <algorithm>

bool BoldRule::Match(std::string_view input, size_t pos) {
  if(input.empty()){
    return false;
  }
  if (pos >= input.size() || input[pos] != '*') {
    return false;
  }

  if (pos + 3 >= input.size()) {
    return false;
  }
  
  if (input[pos] != '*' || input[pos + 1] != '*') {
    return false;
  }

  if (pos + 2 < input.size() && input[pos + 2] == '*') {
    return false;
  }

  size_t closing;
  return HasClosingMarker(input, pos + 2, closing);
}

bool BoldRule::HasClosingMarker(std::string_view input, size_t start, size_t& closingPos) {
  size_t pos = start;
  while (pos + 1 < input.size()) {
    if (input[pos] == '*' && input[pos + 1] == '*') {
      closingPos = pos;
      return true;
    }
    pos++;
  }
  return false;
}

Token BoldRule::Parse(std::string_view input, size_t& pos) {
  size_t startPos = pos;

  size_t closingPos;
  HasClosingMarker(input, pos + 2, closingPos);

  size_t textStart = pos + 2;
  size_t textEnd = closingPos;
  std::string_view boldText = ExtractBoldText(input, textStart, textEnd);

  pos = textEnd + 2;

  return Token(Type::Bold, boldText, "**", startPos, pos);
}

std::string_view BoldRule::ExtractBoldText(std::string_view input, size_t start, size_t& end) {
  if (end <= start) {
    return std::string_view();
  }
  
  return input.substr(start, end - start);
}