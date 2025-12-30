/*
  * lexer.h
  *
  *  Created on: Mar 15, 2024
*/

#ifndef LEXER_H
#define LEXER_H

#include <string_view>
#include <vector>

enum class Type {
  Heading,
  Bold,
  Italic,
  Link,
  Text,
  listItem,
  Image,
  Paragraph,
  Code,
  Quote,
  HorizontalRule,
  EndOfFile
};


struct Token {
  Type type;
  std::string_view value;
  std::string_view meta; // levels, meta data.
  std::vector<Token> children;
  size_t pos;
  size_t max;

  Token() = default;
  Token(Type t, std::string_view v, std::string_view m = {}, size_t ps = 0, size_t max = 0) 
    : type(t), value(v), meta(m), pos(ps), max(max) {}
};

class ILexer {
public:
  virtual ~ILexer() = default;
  virtual std::vector<Token> Tokenize(std::string_view input) = 0;
};

class Lexer : public ILexer {
  public:
    Lexer() = default;
    ~Lexer() override = default;
    std::vector<Token> Tokenize(std::string_view input) override;
};

#endif // LEXER_H