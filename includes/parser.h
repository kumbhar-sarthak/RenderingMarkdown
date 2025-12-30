#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <string>
#include <vector>

class IParser {
public:
    virtual ~IParser() = default;
    virtual std::string Parse(const std::vector<Token>& tokens) = 0;
};

class Parser : public IParser {
public:
    Parser() = default;
    ~Parser() override = default;
    
    std::string Parse(const std::vector<Token>& tokens) override;

private:
    std::string RenderToken(const Token& token);
    std::string ParseHeading(const Token& token);
    std::string ParseBold(const Token& token);
    std::string ParseItalic(const Token& token);
    std::string ParseCode(const Token& token);
    std::string ParseLink(const Token& token);
    std::string ParseList(const Token& token);
    std::string ParseText(const Token& token);
    std::string EscapeHTML(std::string_view text);
    bool IsOrderedList(std::string_view meta);
};

#endif