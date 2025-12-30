#include "link_rule.h"

bool LinkRule::Match(std::string_view input, size_t pos) {
    if (input.empty() || pos >= input.size() || input[pos] != '[') {
        return false;
    }

    if (pos + 5 >= input.size()) {
        return false;
    }

    if (IsEscaped(input, pos)) {
        return false;
    }

    return HasValidLinkStructure(input, pos);
}

Token LinkRule::Parse(std::string_view input, size_t& pos) {
    size_t startPos = pos;

    pos++;

    size_t textEnd;
    std::string_view linkText = ExtractLinkText(input, pos, textEnd);

    pos = textEnd + 1;

    if (pos >= input.size() || input[pos] != '(') {
        pos = startPos + 1;
        return Token(Type::Text, "[", "", startPos, pos);
    }

    pos++;

    size_t urlEnd;
    std::string_view linkUrl = ExtractLinkUrl(input, pos, urlEnd);

    pos = urlEnd + 1;

    return Token(Type::Link, linkText, linkUrl, startPos, pos);
}

bool LinkRule::HasValidLinkStructure(std::string_view input, size_t pos) {
    size_t closingBracket = FindClosingBracket(input, pos + 1);
    if (closingBracket == std::string_view::npos) {
        return false;
    }

    if (closingBracket == pos + 1) {
        return false;
    }

    if (closingBracket + 1 >= input.size() || input[closingBracket + 1] != '(') {
        return false;
    }

    size_t closingParen = FindClosingParen(input, closingBracket + 2);
    if (closingParen == std::string_view::npos) {
        return false;
    }

    return true;
}

std::string_view LinkRule::ExtractLinkText(std::string_view input, size_t start, size_t& end) {
    size_t pos = start;
    int bracketDepth = 1;

    while (pos < input.size() && bracketDepth > 0) {
        if (IsEscaped(input, pos)) {
            pos++;
            continue;
        }

        if (input[pos] == '[') {
            bracketDepth++;
        } else if (input[pos] == ']') {
            bracketDepth--;
            if (bracketDepth == 0) {
                end = pos;
                return input.substr(start, pos - start);
            }
        }

        pos++;
    }

    end = pos;
    return std::string_view();
}

std::string_view LinkRule::ExtractLinkUrl(std::string_view input, size_t start, size_t& end) {
    size_t pos = start;

    while (pos < input.size() && (input[pos] == ' ' || input[pos] == '\t')) {
        pos++;
    }

    size_t urlStart = pos;

    if (pos < input.size() && input[pos] == '<') {
        pos++;
        urlStart = pos;

        while (pos < input.size() && input[pos] != '>') {
            pos++;
        }

        if (pos >= input.size()) {
            end = start;
            return std::string_view();
        }

        size_t urlEnd = pos;
        pos++;

        while (pos < input.size() && (input[pos] == ' ' || input[pos] == '\t')) {
            pos++;
        }

        if (pos >= input.size() || input[pos] != ')') {
            end = start;
            return std::string_view();
        }

        end = pos;
        return input.substr(urlStart, urlEnd - urlStart);
    }

    int parenDepth = 1;

    while (pos < input.size()) {
        if (input[pos] == '\\' && pos + 1 < input.size()) {
            pos += 2;
            continue;
        }

        if (input[pos] == '(') {
            parenDepth++;
        } else if (input[pos] == ')') {
            parenDepth--;
            if (parenDepth == 0) {
                end = pos;

                size_t urlEnd = pos;
                while (urlEnd > urlStart && (input[urlEnd - 1] == ' ' || input[urlEnd - 1] == '\t')) {
                    urlEnd--;
                }

                return input.substr(urlStart, urlEnd - urlStart);
            }
        } else if ((input[pos] == ' ' || input[pos] == '\t') && parenDepth == 1) {
            size_t urlEnd = pos;

            while (pos < input.size() && input[pos] != ')') {
                pos++;
            }

            if (pos >= input.size()) {
                end = start;
                return std::string_view();
            }

            end = pos;
            return input.substr(urlStart, urlEnd - urlStart);
        }

        pos++;
    }

    end = start;
    return std::string_view();
}

size_t LinkRule::FindClosingBracket(std::string_view input, size_t start) {
    size_t pos = start;
    int depth = 1;

    while (pos < input.size()) {
        if (IsEscaped(input, pos)) {
            pos++;
            continue;
        }

        if (input[pos] == '[') {
            depth++;
        } else if (input[pos] == ']') {
            depth--;
            if (depth == 0) {
                return pos;
            }
        }

        pos++;
    }

    return std::string_view::npos;
}

size_t LinkRule::FindClosingParen(std::string_view input, size_t start) {
    size_t pos = start;
    int depth = 1;
    bool inAngleBrackets = false;

    while (pos < input.size()) {
        if (input[pos] == '\\' && pos + 1 < input.size()) {
            pos += 2;
            continue;
        }

        if (input[pos] == '<' && !inAngleBrackets) {
            inAngleBrackets = true;
        } else if (input[pos] == '>' && inAngleBrackets) {
            inAngleBrackets = false;
        } else if (!inAngleBrackets && input[pos] == '(') {
            depth++;
        } else if (!inAngleBrackets && input[pos] == ')') {
            depth--;
            if (depth == 0) {
                return pos;
            }
        }

        pos++;
    }

    return std::string_view::npos;
}

bool LinkRule::IsEscaped(std::string_view input, size_t pos) {
    if (pos == 0) {
        return false;
    }

    int backslashCount = 0;
    size_t checkPos = pos - 1;

    while (checkPos < input.size() && input[checkPos] == '\\') {
        backslashCount++;
        if (checkPos == 0) {
            break;
        }
        checkPos--;
    }

    return (backslashCount % 2) == 1;
}