#ifndef RULE_H
#define RULE_H

#include <string_view>
#include "lexer.h"

class IRule {
public:
  virtual ~IRule() = default;
  virtual bool Match(std::string_view input, size_t pos) = 0;
  virtual Token Parse(std::string_view input, size_t& pos) = 0;
};

#endif // RULE_H