#include "lexer.h"
#include "heading_rule.h"
#include "code_rule.h"
#include "bold_rule.h"
#include "italic_rule.h"
#include "link_rule.h"
#include "list_rule.h"
#include "horizontalline_rule.h"
#include <memory>
#include <vector>
#include <string_view>

std::vector<Token> TokenizeInline(std::string_view input) {
    std::vector<Token> tokens;
    std::vector<std::unique_ptr<IRule>> inlineRules;
    inlineRules.push_back(std::make_unique<CodeRule>());
    inlineRules.push_back(std::make_unique<LinkRule>());
    inlineRules.push_back(std::make_unique<BoldRule>());
    inlineRules.push_back(std::make_unique<ItalicRule>());
    inlineRules.push_back(std::make_unique<HorizontalRule>());

    size_t pos = 0;
    size_t textStart = 0;
    bool inText = false;

    while (pos < input.size()) {
        bool matched = false;
        for (auto& rule : inlineRules) {
            if (rule->Match(input, pos)) {
                if (inText && textStart < pos) {
                    tokens.push_back(Token(Type::Text, input.substr(textStart, pos - textStart), "", textStart, pos));
                    inText = false;
                }

                Token token = rule->Parse(input, pos);
                tokens.push_back(token);
                textStart = pos;
                matched = true;
                break;
            }
        }

        if (!matched) {
            if (!inText) {
                textStart = pos;
                inText = true;
            }
            pos++;
        }
    }

    if (inText && textStart < input.size()) {
        tokens.push_back(Token(Type::Text, input.substr(textStart, input.size() - textStart), "", textStart, input.size()));
    }

    return tokens;
}

std::vector<Token> Lexer::Tokenize(std::string_view input) {
    if(input.empty()) {
        return { Token(Type::EndOfFile, "", "", 0, 0) };
    }

    std::vector<Token> tokens;

    std::vector<std::unique_ptr<IRule>> blockRules;
    blockRules.push_back(std::make_unique<HeadingRule>());
    blockRules.push_back(std::make_unique<ListRule>());

    size_t pos = 0;
    size_t textStart = 0;
    bool inText = false;

    while (pos < input.size()) {
        bool matched = false;

        for (auto& rule : blockRules) {
            if (rule->Match(input, pos)) {
                if (inText && textStart < pos) {
                    std::string_view textContent = input.substr(textStart, pos - textStart);
                    auto inlineTokens = TokenizeInline(textContent);
                    tokens.push_back(Token(Type::Text, textContent, "", textStart, pos));
                    tokens.back().children = std::move(inlineTokens);
                    inText = false;
                }

                Token token = rule->Parse(input, pos);

                if (token.type == Type::Heading || token.type == Type::listItem) {
                    auto inlineTokens = TokenizeInline(token.value);
                    token.children = std::move(inlineTokens);
                }

                tokens.push_back(token);
                textStart = pos;
                matched = true;
                break;
            }
        }

        if (!matched) {
            if (!inText) {
                textStart = pos;
                inText = true;
            }
            pos++;
        }
    }

    if (inText && textStart < input.size()) {
        std::string_view textContent = input.substr(textStart, input.size() - textStart);
        auto inlineTokens = TokenizeInline(textContent);
        Token token(Type::Text, textContent, "", textStart, input.size());
        token.children = std::move(inlineTokens);
        tokens.push_back(std::move(token));
    }

    tokens.push_back(Token(Type::EndOfFile, "", "", input.size(), input.size()));

    return tokens;
}
