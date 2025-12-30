#include "list_rule.h"

bool ListRule::Match(std::string_view input, size_t pos) {
  if (pos >= input.size()) {
    return false;
  }

  if (input.empty()) {
    return false;
  }
  
  if (!IsAtLineStart(input, pos)) {
    return false;
  }
  
  size_t spaceCount = CountLeadingSpaces(input, pos);
  pos += spaceCount;
  
  if (pos >= input.size()) {
    return false;
  }
  
  return IsUnorderedList(input, pos) || IsOrderedList(input, pos);
}

Token ListRule::Parse(std::string_view input, size_t& pos) {
  size_t startPos = pos;
  
  size_t spaceCount = CountLeadingSpaces(input, pos);
  pos += spaceCount;
  
  size_t markerEnd;
  std::string_view marker = ExtractListMarker(input, pos, markerEnd);
  pos = markerEnd;
  
  if (pos < input.size() && (input[pos] == ' ' || input[pos] == '\t')) {
    pos++;
  }
  
  size_t contentEnd;
  std::string_view content = ExtractListContent(input, pos, contentEnd);
  pos = contentEnd;
  
  if (pos < input.size() && input[pos] == '\n') {
    pos++;
  }
  
  return Token(Type::listItem, content, marker, startPos, pos);
}

bool ListRule::IsUnorderedList(std::string_view input, size_t pos) {
  if (pos >= input.size()) {
    return false;
  }
  
  char c = input[pos];
  if (c != '-' && c != '*' && c != '+') {
    return false;
  }
  
  if (pos + 1 >= input.size()) {
    return false;
  }
  
  return input[pos + 1] == ' ' || input[pos + 1] == '\t' || input[pos + 1] == '\n';
}

bool ListRule::IsOrderedList(std::string_view input, size_t pos) {
  if (pos >= input.size() || !IsDigit(input[pos])) {
    return false;
  }
  
  size_t digitPos = pos;
  while (digitPos < input.size() && IsDigit(input[digitPos])) {
    digitPos++;
  }
  
  if (digitPos >= input.size() || input[digitPos] != '.') {
    return false;
  }
  
  if (digitPos + 1 >= input.size()) {
    return false;
  }
  
  return input[digitPos + 1] == ' ' || input[digitPos + 1] == '\t' || input[digitPos + 1] == '\n';
}

std::string_view ListRule::ExtractListContent(std::string_view input, size_t start, size_t& end) {
  end = start;
  
  while (end < input.size() && input[end] != '\n') {
    end++;
  }
  
  size_t contentEnd = end;
  while (contentEnd > start && (input[contentEnd - 1] == ' ' || input[contentEnd - 1] == '\t')) {
    contentEnd--;
  }
  
  if (contentEnd <= start) {
    return std::string_view();
  }
  
  return input.substr(start, contentEnd - start);
}

std::string_view ListRule::ExtractListMarker(std::string_view input, size_t pos, size_t& markerEnd) {
  size_t start = pos;
  
  if (pos < input.size() && (input[pos] == '-' || input[pos] == '*' || input[pos] == '+')) {
    markerEnd = pos + 1;
    return input.substr(start, 1);
  }
  // for ordered list
  if (pos < input.size() && IsDigit(input[pos])) {
    while (pos < input.size() && IsDigit(input[pos])) {
      pos++;
    }
    // check for dot
    if (pos < input.size() && input[pos] == '.') {
      pos++;
      markerEnd = pos;
      return input.substr(start, pos - start);
    }
  }
  
  markerEnd = start;
  return std::string_view();
}

bool ListRule::IsAtLineStart(std::string_view input, size_t pos) {
  if (pos == 0) {
    return true;
  }
  
  return input[pos - 1] == '\n';
}

bool ListRule::IsDigit(char c) {
  return c >= '0' && c <= '9';
}

size_t ListRule::CountLeadingSpaces(std::string_view input, size_t pos) {
  size_t count = 0;
  while (pos + count < input.size() && 
         (input[pos + count] == ' ' || input[pos + count] == '\t')) {
    count++;
  }
  return count;
}