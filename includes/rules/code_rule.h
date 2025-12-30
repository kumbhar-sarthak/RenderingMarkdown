#ifndef CODE_RULE_H
#define CODE_RULE_H

#include "rule.h"
#include "lexer.h"
#include <string_view>

class CodeRule : public IRule {
public:
  CodeRule() = default;
  ~CodeRule() override = default;
  
  bool Match(std::string_view input, size_t pos) override;
  Token Parse(std::string_view input, size_t& pos) override;

private:
  bool IsCodeBlock(std::string_view input, size_t pos);
  bool IsInlineCode(std::string_view input, size_t pos);
  
  Token ParseCodeBlock(std::string_view input, size_t& pos);
  Token ParseInlineCode(std::string_view input, size_t& pos);
  
  size_t CountBackticks(std::string_view input, size_t pos);
  std::string_view ExtractLanguage(std::string_view input, size_t start, size_t& end);
  std::string_view ExtractCodeContent(std::string_view input, size_t start, size_t end);
  
  bool IsAtLineStart(std::string_view input, size_t pos);
};

#endif