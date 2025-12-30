#ifndef LIST_RULE_H
#define LIST_RULE_H

#include "rule.h"
#include "lexer.h"
#include <string_view>

class ListRule : public IRule {
public:
  ListRule() = default;
  ~ListRule() override = default;
  
  bool Match(std::string_view input, size_t pos) override;
  Token Parse(std::string_view input, size_t& pos) override;

private:
  bool IsUnorderedList(std::string_view input, size_t pos);
  bool IsOrderedList(std::string_view input, size_t pos);
  
  std::string_view ExtractListContent(std::string_view input, size_t start, size_t& end);
  std::string_view ExtractListMarker(std::string_view input, size_t pos, size_t& markerEnd);
  
  bool IsAtLineStart(std::string_view input, size_t pos);
  bool IsDigit(char c);
  size_t CountLeadingSpaces(std::string_view input, size_t pos);
};

#endif