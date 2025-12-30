#ifndef HORIZONTAL_RULE_H
#define HORIZONTAL_RULE_H

#include "rule.h"
#include "lexer.h"
#include <string_view>

class HorizontalRule : public IRule {
public:
  HorizontalRule() = default;
  ~HorizontalRule() override = default;
  
  bool Match(std::string_view input, size_t pos) override;
  Token Parse(std::string_view input, size_t& pos) override;

private:
  bool IsAtLineStart(std::string_view input, size_t pos);
  bool IsHorizontalRule(std::string_view input, size_t pos);
  size_t CountCharacter(std::string_view input, size_t pos, char c);
};

#endif