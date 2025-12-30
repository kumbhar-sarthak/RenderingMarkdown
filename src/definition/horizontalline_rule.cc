#include "horizontalline_rule.h"

bool HorizontalRule::Match(std::string_view input, size_t pos) {
    if (!IsAtLineStart(input, pos)) {
        return false;
    }
    
    if (pos >= input.size()) {
        return false;
    }
    
    char c = input[pos];
    if (c != '-' && c != '*' && c != '_') {
        return false;
    }
    
    return IsHorizontalRule(input, pos);
}

Token HorizontalRule::Parse(std::string_view input, size_t& pos) {
    size_t startPos = pos;
    
    char c = input[pos];
    while (pos < input.size() && (input[pos] == c || input[pos] == ' ' || input[pos] == '\t')) {
        pos++;
    }
    
    if (pos < input.size() && input[pos] == '\n') {
        pos++;
    }
    
    return Token(Type::HorizontalRule, "", "", startPos, pos);
}

bool HorizontalRule::IsAtLineStart(std::string_view input, size_t pos) {
    if (pos == 0) {
        return true;
    }
    return input[pos - 1] == '\n';
}

bool HorizontalRule::IsHorizontalRule(std::string_view input, size_t pos) {
    char c = input[pos];
    size_t count = 0;
    size_t tempPos = pos;

    while (tempPos < input.size() && input[tempPos] != '\n') {
        if (input[tempPos] == c) {
            count++;
        } else if (input[tempPos] != ' ' && input[tempPos] != '\t') {
            return false;
        }
        tempPos++;
    }
  
    return count >= 3;
}

size_t HorizontalRule::CountCharacter(std::string_view input, size_t pos, char c) {
    size_t count = 0;
    while (pos < input.size() && input[pos] == c) {
        count++;
        pos++;
    }
    return count;
}