#ifndef LINK_RULE_H
#define LINK_RULE_H

#include "rule.h"
#include "lexer.h"
#include <string_view>

class LinkRule : public IRule {
public:
  LinkRule() = default;
  ~LinkRule() override = default;
  
  bool Match(std::string_view input, size_t pos) override;
  Token Parse(std::string_view input, size_t& pos) override;

private:
  bool HasValidLinkStructure(std::string_view input, size_t pos);
  std::string_view ExtractLinkText(std::string_view input, size_t start, size_t& end);
  std::string_view ExtractLinkUrl(std::string_view input, size_t start, size_t& end);
  
  size_t FindClosingBracket(std::string_view input, size_t start);
  size_t FindClosingParen(std::string_view input, size_t start);
  
  bool IsEscaped(std::string_view input, size_t pos);
};

#endif