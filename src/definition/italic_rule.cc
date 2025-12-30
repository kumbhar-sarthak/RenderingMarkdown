#include "italic_rule.h"
#include <algorithm>

bool ItalicRule::Match(std::string_view input, size_t pos) {

  if(input.empty()){
    return false;
  }

  if (pos + 1 >= input.size()) {
    return false;
  }

  if (input[pos] != '*' && input[pos] != '_') {
    return false;
  }

  char marker = input[pos];
  size_t endPos = input.find(marker, pos + 1);
  if (endPos == std::string_view::npos || endPos == pos + 1) {
    return false;
  }

  return true;
}

Token ItalicRule::Parse(std::string_view input, size_t& pos) {
  size_t startPos = pos;

  char marker = input[pos];
  pos += 1; 

  size_t textStart = pos;
  size_t textEnd = pos;
  std::string_view italicText = ExtractItalicText(input, textStart, textEnd);

  pos = textEnd + 1;

  return Token(Type::Italic, italicText, std::string_view(&marker, 1), startPos, pos);
}

std::string_view ItalicRule::ExtractItalicText(std::string_view input, size_t start, size_t& end) {
  if (end <= start) {
    return std::string_view();
  }

  end = input.find_first_of(" \n", start);
  if (end == std::string_view::npos) {
    end = input.size();
  }

  return input.substr(start, end - start);
}

bool ItalicRule::HashsClosingMarker(std::string_view input, size_t start, size_t& closingPos) {
  size_t pos = start;
  char marker = input[start];
  while (pos < input.size()) {
    if (input[pos] == marker) {
      closingPos = pos;
      return true;
    }
    pos++;
  }
  return false;
}