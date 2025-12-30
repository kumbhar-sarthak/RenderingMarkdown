#ifndef BOLD_RULE_H
#define BOLD_RULE_H

#include "rule.h"
#include "lexer.h"
#include <string_view>

class BoldRule : public IRule {
public:
  BoldRule() = default;
  ~BoldRule() override = default;

  bool Match(std::string_view input, size_t pos) override;
  Token Parse(std::string_view input, size_t& pos) override;

private:
  std::string_view ExtractBoldText(std::string_view input, size_t start, size_t& end);
  bool HasClosingMarker(std::string_view input, size_t start, size_t& closingPos);
};

#endif // BOLD_RULE_H