#include "code_rule.h"

bool CodeRule::Match(std::string_view input, size_t pos) {
  if (pos >= input.size() || input[pos] != '`') {
    return false;
  }

  if (input.empty()) {
    return false;
  }
  
  return IsCodeBlock(input, pos) || IsInlineCode(input, pos);
}

Token CodeRule::Parse(std::string_view input, size_t& pos) {
  if (IsCodeBlock(input, pos)) {
    return ParseCodeBlock(input, pos);
  } else {
    return ParseInlineCode(input, pos);
  }
}

bool CodeRule::IsCodeBlock(std::string_view input, size_t pos) {
  if (!IsAtLineStart(input, pos)) {
    return false;
  }
  
  return CountBackticks(input, pos) >= 3;
}

bool CodeRule::IsInlineCode(std::string_view input, size_t pos) {
  size_t backtickCount = CountBackticks(input, pos);
  
  if (backtickCount == 0 || backtickCount >= 3) {
    return false;
  }
  
  size_t searchPos = pos + backtickCount;
  while (searchPos < input.size()) {
    if (input[searchPos] == '`') {
      size_t closingCount = CountBackticks(input, searchPos);
      if (closingCount == backtickCount) {
        return true;
      }
      searchPos += closingCount;
    } else if (input[searchPos] == '\n') {
      return false;
    } else {
      searchPos++;
    }
  }
  
  return false;
}

Token CodeRule::ParseCodeBlock(std::string_view input, size_t& pos) {
  size_t startPos = pos;
  size_t backtickCount = CountBackticks(input, pos);
  pos += backtickCount;
  
  size_t langStart = pos;
  size_t langEnd = langStart;
  std::string_view language = ExtractLanguage(input, langStart, langEnd);
  pos = langEnd;
  
  if (pos < input.size() && input[pos] == '\n') {
    pos++;
  }
  
  size_t contentStart = pos;
  size_t contentEnd = pos;
  
  while (contentEnd < input.size()) {
    if (contentEnd == contentStart || input[contentEnd - 1] == '\n') {
      size_t closingCount = CountBackticks(input, contentEnd);
      if (closingCount == backtickCount) {
        size_t codeEnd = contentEnd;
        
        if (codeEnd > contentStart && input[codeEnd - 1] == '\n') {
          codeEnd--;
        }
        
        std::string_view codeContent = ExtractCodeContent(input, contentStart, codeEnd);
        
        pos = contentEnd + closingCount;
        
        if (pos < input.size() && input[pos] == '\n') {
          pos++;
        }
        
        return Token(Type::Code, codeContent, language, startPos, pos);
      }
    }
    contentEnd++;
  }
  
  std::string_view codeContent = ExtractCodeContent(input, contentStart, input.size());
  pos = input.size();
  
  return Token(Type::Code, codeContent, language, startPos, pos);
}

Token CodeRule::ParseInlineCode(std::string_view input, size_t& pos) {
  size_t startPos = pos;
  size_t backtickCount = CountBackticks(input, pos);
  pos += backtickCount;
  
  size_t contentStart = pos;
  
  while (pos < input.size()) {
    if (input[pos] == '`') {
      size_t closingCount = CountBackticks(input, pos);
      if (closingCount == backtickCount) {
        std::string_view codeContent = input.substr(contentStart, pos - contentStart);
        pos += closingCount;
        
        return Token(Type::Code, codeContent, "", startPos, pos);
      }
      pos += closingCount;
    } else {
      pos++;
    }
  }
  
  std::string_view codeContent = input.substr(contentStart, pos - contentStart);
  return Token(Type::Code, codeContent, "", startPos, pos);
}

size_t CodeRule::CountBackticks(std::string_view input, size_t pos) {
  size_t count = 0;
  while (pos + count < input.size() && input[pos + count] == '`') {
    count++;
  }
  return count;
}

std::string_view CodeRule::ExtractLanguage(std::string_view input, size_t start, size_t& end) {
  end = start;
  
  while (end < input.size() && (input[end] == ' ' || input[end] == '\t')) {
    end++;
  }
  
  size_t langStart = end;
  
  while (end < input.size() && input[end] != '\n' && input[end] != ' ' && input[end] != '\t') {
    end++;
  }
  
  while (end < input.size() && input[end] != '\n') {
    end++;
  }
  
  if (end > langStart) {
    return input.substr(langStart, end - langStart);
  }
  
  return std::string_view();
}

std::string_view CodeRule::ExtractCodeContent(std::string_view input, size_t start, size_t end) {
  if (end <= start) {
    return std::string_view();
  }
  
  return input.substr(start, end - start);
}

bool CodeRule::IsAtLineStart(std::string_view input, size_t pos) {
  if (pos == 0) {
    return true;
  }
  
  return input[pos - 1] == '\n';
}