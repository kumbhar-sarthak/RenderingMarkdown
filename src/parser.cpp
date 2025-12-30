#include "parser.h"
#include <sstream>
#include <cctype>

std::string Parser::RenderToken(const Token &token)
{
    std::ostringstream html;

    switch (token.type)
    {
    case Type::Heading:
        html << ParseHeading(token);
        break;

    case Type::listItem:
        html << ParseList(token);
        break;

    case Type::Text:
        html << ParseText(token);
        break;

    case Type::Bold:
        html << ParseBold(token);
        break;

    case Type::Italic:
        html << ParseItalic(token);
        break;

    case Type::Code:
        html << ParseCode(token);
        break;

    case Type::Link:
        html << ParseLink(token);
        break;

    default:
        html << EscapeHTML(token.value);
        break;
    }

    return html.str();
}

std::string Parser::Parse(const std::vector<Token> &tokens)
{
    std::ostringstream html;
    bool inParagraph = false;
    bool inList = false;
    bool isOrderedList = false;

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        const auto &token = tokens[i];

        // Close list if current token is not a list item
        if (token.type != Type::listItem && inList)
        {
            html << (isOrderedList ? "</ol>\n" : "</ul>\n");
            inList = false;
        }

        switch (token.type)
        {
        case Type::Heading:
            if (inParagraph)
            {
                html << "</p>\n";
                inParagraph = false;
            }
            html << RenderToken(token);
            break;

        case Type::listItem:
        {
            bool currentIsOrdered = IsOrderedList(token.meta);

            if (!inList)
            {
                html << (currentIsOrdered ? "<ol>\n" : "<ul>\n");
                inList = true;
                isOrderedList = currentIsOrdered;
            }

            else if (currentIsOrdered != isOrderedList)
            {
                html << (isOrderedList ? "</ol>\n" : "</ul>\n");
                html << (currentIsOrdered ? "<ol>\n" : "<ul>\n");
                isOrderedList = currentIsOrdered;
            }

            html << RenderToken(token);
            break;
        }

        case Type::Text:
            if (inParagraph)
            {
                html << "</p>\n";
                inParagraph = false;
            }
            if (!token.value.empty() && token.value != "\n")
            {
                if (!inParagraph)
                {
                    html << "<p>";
                    inParagraph = true;
                }
                html << RenderToken(token);
            }
            break;

        case Type::Code:
            if (inParagraph)
            {
                html << "</p>\n";
                inParagraph = false;
            }
            html << RenderToken(token);
            break;

        case Type::Bold:
            if (!inParagraph)
            {
                html << "<p>";
                inParagraph = true;
            }
            html << RenderToken(token);
            break;

        case Type::Italic:
            if (!inParagraph)
            {
                html << "<p>";
                inParagraph = true;
            }
            html << RenderToken(token);
            break;

        case Type::Link:
            if (!inParagraph)
            {
                html << "<p>";
                inParagraph = true;
            }
            html << RenderToken(token);
            break;

        case Type::EndOfFile:
            if (inList)
            {
                html << (isOrderedList ? "</ol>\n" : "</ul>\n");
                inList = false;
            }
            if (inParagraph)
            {
                html << "</p>\n";
                inParagraph = false;
            }
            break;

        case Type::HorizontalRule:
            if (inParagraph)
            {
                html << "</p>\n";
                inParagraph = false;
            }
            html << "<hr style= />\n";
            break;

        default:
            break;
        }
    }

    return html.str();
}

std::string Parser::ParseHeading(const Token &token)
{
    int level = token.meta.length();
    return "<h" + std::to_string(level) + ">" +
           EscapeHTML(token.value) +
           "</h" + std::to_string(level) + ">\n";
}

std::string Parser::ParseBold(const Token &token)
{
    return "<strong>" + EscapeHTML(token.value) + "</strong>";
}

std::string Parser::ParseItalic(const Token &token)
{
    return "<em>" + EscapeHTML(token.value) + "</em>";
}

std::string Parser::ParseCode(const Token &token)
{
    std::ostringstream html;
    if (token.meta.empty())
    {
        html << "<code>" << EscapeHTML(token.value) << "</code>";
    }
    else
    {
        html << "<pre><code class=\"language-" << std::string(token.meta) << "\">"
             << EscapeHTML(token.value) << "</code></pre>\n";
    }
    return html.str();
}

std::string Parser::ParseLink(const Token &token)
{
    return "<a href=\"" + EscapeHTML(token.meta) + "\">" +
           EscapeHTML(token.value) + "</a>";
}

std::string Parser::ParseList(const Token &token)
{
    std::ostringstream html;
    html << "<li>";
    if (!token.children.empty())
    {
        for (const auto &child : token.children)
            html << RenderToken(child);
    }
    else
    {
        html << EscapeHTML(token.value);
    }
    html << "</li>\n";
    return html.str();
}

std::string Parser::ParseText(const Token &token)
{
    std::ostringstream html;
    if (!token.children.empty())
    {
        for (const auto &child : token.children)
            html << RenderToken(child);
    }
    else
    {
        html << EscapeHTML(token.value);
    }
    return html.str();
}

std::string Parser::EscapeHTML(std::string_view text)
{
    std::string result;
    result.reserve(text.size());

    for (char c : text)
    {
        switch (c)
        {
        case '<':
            result += "&lt;";
            break;
        case '>':
            result += "&gt;";
            break;
        case '&':
            result += "&amp;";
            break;
        case '"':
            result += "&quot;";
            break;
        case '\'':
            result += "&#39;";
            break;
        default:
            result += c;
        }
    }

    return result;
}

bool Parser::IsOrderedList(std::string_view meta)
{
    if (meta.empty())
    {
        return false;
    }

    return std::isdigit(static_cast<unsigned char>(meta[0]));
}