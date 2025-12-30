#ifndef ITALIC_RULE_H
#define ITALIC_RULE_H

#include "rule.h"
#include "lexer.h"
#include <string_view>

class ItalicRule : public IRule {
public:
  ItalicRule() = default;
  ~ItalicRule() override = default;
  
  bool Match(std::string_view input, size_t pos) override;
  Token Parse(std::string_view input, size_t& pos) override;

private:
  std::string_view ExtractItalicText(std::string_view input, size_t start, size_t& end);
  bool HashsClosingMarker(std::string_view input, size_t start, size_t& closingPos);
};

#endif // ITALIC_RULE_H