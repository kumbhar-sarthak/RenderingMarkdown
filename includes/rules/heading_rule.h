#ifndef HEADING_RULE_H
#define HEADING_RULE_H

#include "rule.h"
#include "lexer.h"
#include <string_view>

class HeadingRule : public IRule {
public:
  HeadingRule() = default;
  ~HeadingRule() override = default;
  
  bool Match(std::string_view input, size_t pos) override;
  Token Parse(std::string_view input, size_t& pos) override;

private:
  size_t CountHashes(std::string_view input, size_t pos);
  std::string_view ExtractHeadingText(std::string_view input, size_t start, size_t& end);
};

#endif // HEADING_RULE_H