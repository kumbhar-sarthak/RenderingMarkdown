#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <filesystem>
#include <atomic>
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
    std::atomic<int> i{0};
    std::string css()
    {
        std::ifstream cssFile("utils/formatting.css");
        if (!cssFile.is_open())
        {
            return "/* Could not load CSS file */";
        }
        std::stringstream buffer;
        buffer << cssFile.rdbuf();
        return buffer.str();
    }

    std::string fileSize(const FileContent &fileContent)
    {
        const char *units[] = {"B", "KB", "MB", "GB", "TB"};
        size_t size = fileContent.content.size();
        int unitIndex = 0;

        while (size >= 1024 && unitIndex < 4)
        {
            size /= 1024;
            ++unitIndex;
        }

        return std::to_string(size) + " " + units[unitIndex];
    }

    [[nodiscard]] FileContent readFile(const std::string &filename)
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

    Manager(const Manager &other) = delete;

    Manager &operator=(const Manager &other) = delete;

    Manager(Manager &&other) noexcept = default;

    Manager &operator=(Manager &&other) noexcept
    {
        if (this != &other)
        {
        }
        return *this;
    };

    void ProcessFile(const std::string &filename)
    {
        FileContent fileContent = readFile(filename);

        if (!fileContent.success)
        {
            std::cerr << "Thread " << std::this_thread::get_id()
                      << ": Error: " << fileContent.error << "\n";
            return;
        }

        std::cout << ": Processing file: " << std::filesystem::path(filename).filename().string()
                  << ", size: " << fileSize(fileContent) << "\n";

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

        std::string outputfile = "target/result";
        outputfile += std::to_string(++i);
        outputfile += ".html";

        writeToFile(outputfile, output);
    }
};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <markdown_file1> [markdown_file2] ...\n";
        std::cerr << "Example: " << argv[0] << " document.md\n";
        return 1;
    }

    std::cout << "Markdown Parser - Processing " << (argc - 1) << " file(s)\n";
    std::cout << "Main thread: " << std::this_thread::get_id() << "\n\n";

    auto shareManager = std::make_shared<Manager>();
    std::vector<std::thread> threads;

    for (int i = 1; i < argc; ++i)
    {
        threads.emplace_back(&Manager::ProcessFile, shareManager, argv[i]);
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    std::cout << "\nAll files processed successfully.\n";

    return 0;
}