#include "heading_rule.h"
#include <algorithm>

bool HeadingRule::Match(std::string_view input, size_t pos) {

  if(input.empty()){
    return false;
  }

  if (pos > 0 && input[pos - 1] != '\n') {
    return false;
  }
  
  if (pos >= input.size() || input[pos] != '#') {
    return false;
  }
  
  size_t hashCount = CountHashes(input, pos);
  
  if (hashCount < 1 || hashCount > 6) {
    return false;
  }
  
  size_t afterHashes = pos + hashCount;
  if (afterHashes >= input.size()) {
    return true;
  }
  
  return input[afterHashes] == ' ' || input[afterHashes] == '\n';
}

Token HeadingRule::Parse(std::string_view input, size_t& pos) {
  size_t startPos = pos;
  
  size_t level = CountHashes(input, pos);
  pos += level;
  
  if (pos < input.size() && input[pos] == ' ') {
    pos++;
  }
  
  size_t textStart = pos;
  size_t textEnd = pos;
  std::string_view headingText = ExtractHeadingText(input, textStart, textEnd);
  
  pos = textEnd;
  
  if (pos < input.size() && input[pos] == '\n') {
    pos++;
  }

  std::string_view levelStr = input.substr(startPos, level);
  
  return Token(Type::Heading, headingText, levelStr, startPos, pos);
}

size_t HeadingRule::CountHashes(std::string_view input, size_t pos) {
  size_t count = 0;
  while (pos + count < input.size() && input[pos + count] == '#' && count < 6) {
    count++;
  }
  return count;
}

std::string_view HeadingRule::ExtractHeadingText(std::string_view input, size_t start, size_t& end) {
  end = start;
  
  while (end < input.size() && input[end] != '\n') {
    end++;
  }
  
  size_t contentEnd = end;
  while (contentEnd > start && (input[contentEnd - 1] == ' ' || input[contentEnd - 1] == '#')) {
    contentEnd--;
  }
  
  if (contentEnd <= start) {
    return std::string_view();
  }
  
  return input.substr(start, contentEnd - start);
}