#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <filesystem>
#include "lexer.h"
#include "parser.h"

struct FileContent
{
    std::string content;
    bool success;
    std::string error;
};

class Manager
{
    std::string css()
    {
        return R"(
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
            line-height: 1.6;
            max-width: 800px;
            margin: 0 auto;
            padding: 20px;
            color: #333;
            background-color: #fff;
        }
        
        h1, h2, h3, h4, h5, h6 {
            margin-top: 24px;
            margin-bottom: 16px;
            font-weight: 600;
            line-height: 1.25;
        }
        
        h1 {
            font-size: 2em;
            border-bottom: 1px solid #eaecef;
            padding-bottom: 0.3em;
        }
        
        h2 {
            font-size: 1.5em;
            border-bottom: 1px solid #eaecef;
            padding-bottom: 0.3em;
        }
        
        h3 { font-size: 1.25em; }
        h4 { font-size: 1em; }
        h5 { font-size: 0.875em; }
        h6 { font-size: 0.85em; color: #6a737d; }
        
        p {
            margin-top: 0;
            margin-bottom: 16px;
        }
        
        a {
            color: #0366d6;
            text-decoration: none;
        }
        
        a:hover {
            text-decoration: underline;
        }
        
        strong {
            font-weight: 600;
        }
        
        em {
            font-style: italic;
        }
        
        code {
            padding: .2em .4em;
            margin: 0;
            font-size: 85%;
            white-space: break-spaces;
            background-color: #818b981f;
            border-radius: 6px;
        }
        
        pre {
            padding: 16px;
            overflow: auto;
            font-size: 85%;
            line-height: 1.45;
            background-color: #f6f8fa;
            border-radius: 6px;
            margin-bottom: 16px;
        }
        
        pre code {
            display: inline;
            padding: 0;
            margin: 0;
            overflow: visible;
            line-height: inherit;
            background-color: transparent;
            border: 0;
        }
        
        ul, ol {
            padding-left: 2em;
            margin-top: 0;
            margin-bottom: 16px;
        }
        
        li {
            margin-bottom: 0.25em;
        }
        
        li > p {
            margin-top: 16px;
        }
        
        hr {
            height: 0.25em;
            padding: 0;
            margin: 24px 0;
            background-color: #e1e4e8;
            border: 0;
        }
        
        blockquote {
            padding: 0 1em;
            color: #6a737d;
            border-left: 0.25em solid #dfe2e5;
            margin: 0 0 16px 0;
        }
        
        table {
            border-spacing: 0;
            border-collapse: collapse;
            margin-bottom: 16px;
        }
        
        table th, table td {
            padding: 6px 13px;
            border: 1px solid #dfe2e5;
        }
        
        table tr {
            background-color: #fff;
            border-top: 1px solid #c6cbd1;
        }
        
        table tr:nth-child(2n) {
            background-color: #f6f8fa;
        }
    )";
    }

    std::string fileSize(const FileContent &fileContent) {
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        size_t size = fileContent.content.size();
        int unitIndex = 0;

        while (size >= 1024 && unitIndex < 4) {
            size /= 1024;
            ++unitIndex;
        }

        return std::to_string(size) + " " + units[unitIndex];
    }

    FileContent readFile(const std::string &filename)
    {
        FileContent result;
        result.success = false;

        // remove path problems
        std::filesystem::path filepath = std::filesystem::absolute(filename);

        if (std::filesystem::exists(filepath) == false)
        {
            result.error = "Path does not exist: " + filename;
            return result;
        }

        std::ifstream file(filepath);
        if (!file.is_open())
        {
            result.error = "Could not open file: " + filename;
            return result;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        result.content = buffer.str();
        result.success = true;

        return result;
    }

    void printTokens(const std::vector<Token> &tokens)
    {
        const char *typeNames[] = {
            "Heading",
            "Bold",
            "Italic",
            "Link",
            "Text",
            "ListItem",
            "Image",
            "Paragraph",
            "Code",
            "Quote",
            "HorizontalRule",
            "EndOfFile"};

        std::cout << "\n=== Tokenization Results ===\n";
        std::cout << "Total tokens: " << tokens.size() << "\n\n";

        for (const auto &token : tokens)
        {
            std::cout << "Type: " << typeNames[static_cast<int>(token.type)];

            if (!token.value.empty())
            {
                std::cout << " | Value: \"";
                if (token.value.length() > 50)
                {
                    std::cout << token.value.substr(0, 47) << "...";
                }
                else
                {
                    std::cout << token.value;
                }
                std::cout << "\"";
            }

            if (!token.meta.empty())
            {
                std::cout << " | Meta: \"" << token.meta << "\"";
            }

            std::cout << " | Pos: [" << token.pos << "-" << token.max << "]\n";

            if (!token.children.empty())
            {
                std::cout << "  Children Tokens: ";
                for (const auto &child : token.children)
                {
                    std::cout << "    Type: " << typeNames[static_cast<int>(child.type)];

                    if (!child.value.empty())
                    {
                        std::cout << " | Value: ";
                        if (child.value.length() > 50)
                        {
                            std::cout << child.value.substr(0, 47) << "...";
                        }
                        else
                        {
                            std::cout << child.value;
                        }
                    }

                    if (!child.meta.empty())
                    {
                        std::cout << " | Meta: \"" << child.meta << "\"";
                    }

                    std::cout << " | Pos: [" << child.pos << "-" << child.max << "]\n";
                }
            }
        }
    }

    void printParser(const std::string &output)
    {
        std::cout << output << std::endl;
    }

    void writeToFile(const std::string &filename, const std::string &content)
    {
        std::filesystem::path filePath = std::filesystem::absolute(filename);
        std::ofstream outFile(filePath);
        if (outFile.is_open())
        {
            outFile << content;
            outFile.close();
        }
        else
        {
            std::cerr << "Error: Could not write to file: " << filename << "\n";
        }
    }

public:
    Manager() = default;
    ~Manager() = default;

    void ProcessFile(const std::string &filename, int i)
    {
        std::cout << "Thread " << std::this_thread::get_id()
                  << ": Processing file: " << filename << "\n";

        FileContent fileContent = readFile(filename);

        if (!fileContent.success)
        {
            std::cerr << "Thread " << std::this_thread::get_id()
                      << ": Error: " << fileContent.error << "\n";
            return;
        }

        std::cout << "Thread " << std::this_thread::get_id()
                  << ": File size: " << fileSize(fileContent) << "\n";

        Lexer lexer;
        std::vector<Token> tokens = lexer.Tokenize(fileContent.content);

        Parser parser;
        std::string html = parser.Parse(tokens);

        std::string output = "<!DOCTYPE html>\n";
        output += "<html>\n<head>\n";
        output += "<meta charset=\"UTF-8\">\n";
        output += "<title>Markdown Output</title>\n";
        output += "<style>\n" + css() + "\n</style>\n";
        output += "</head>\n<body>\n";
        output += html;
        output += "</body>\n</html>\n";

        std::string outputfile = "tests/result";
        outputfile += std::to_string(i);
        outputfile += ".html";

        writeToFile(outputfile, output);
    }
};

int
main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <markdown_file1> [markdown_file2] ...\n";
        std::cerr << "Example: " << argv[0] << " document.md\n";
        return 1;
    }

    std::cout << "Markdown Parser - Processing " << (argc - 1) << " file(s)\n";
    std::cout << "Main thread: " << std::this_thread::get_id() << "\n\n";

    Manager manage;
    std::vector<std::thread> threads;

    for (int i = 1; i < argc; ++i)
    {
        threads.emplace_back(&Manager::ProcessFile, &manage, argv[i], i);
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    std::cout << "\nAll files processed successfully.\n";

    return 0;
}