#include <iostream>
#include <regex>
#include <fstream>

void lexRegexAnalyze(const std::string &jsCode) {
    std::regex whitespaceRegex("(\\s+)");
    std::regex keywordRegex("\\b(function|var|return|if|else|while|for)\\b");
    std::regex identifierRegex("\\b([a-zA-Z_][a-zA-Z0-9_]*)\\b");
    std::regex dotRegex("\\.");
    std::regex numericLiteralRegex("([0-9]+)");
    std::regex stringLiteralRegex("(\"(.*?)\")");
    std::regex charLiteralRegex("'.'");
    std::regex preprocessorDirectiveRegex("#\\w+");
    std::regex commentRegex("//.*|/\\*.*?\\*/");
    std::regex operatorRegex("(==|!=|<=|>=|&&|\\|\\||\\+\\+|--|\\+|-|\\*|/|%|&|\\||\\^|~|<<|>>|!|=)");
    std::regex punctuationRegex("(\\{|\\}|\\(|\\)|\\[|\\]|;|,)");

    std::string remainingCode = jsCode;

    while (!remainingCode.empty()) {
        std::smatch match;
        bool found = false;

        // List of all regex patterns
        std::vector<std::pair<std::regex, std::string>> patterns = {
                {whitespaceRegex, "Whitespace"},
                {keywordRegex, "Keyword"},
                {identifierRegex, "Identifier"},
                {numericLiteralRegex, "Numeric Literal"},
                {stringLiteralRegex, "String Literal"},
                {charLiteralRegex, "Char Literal"},
                {preprocessorDirectiveRegex, "Preprocessor directive"},
                {commentRegex, "Comment"},
                {operatorRegex, "Operator"},
                {punctuationRegex, "Punctuation"},
                {dotRegex, "Dot Operator"}
        };

        for (const auto& [regex, type] : patterns) {
            if (std::regex_search(remainingCode, match, regex)) {
                if(match.position() == 0){
                    if (type != "Whitespace") {  // Ignore whitespace
                        std::cout << match[0] << " - " << type << "\n";
                    }
                    remainingCode = match.suffix().str();
                    found = true;
                    break;
                }
            }
        }

        // No more matches
        if (!found) {
            std::cerr << "Unrecognized sequence: " << remainingCode << "\n";
            break;
        }
    }
}

int main() {
    std::ifstream file("/Users/maksymkhodakov/CLionProjects/JSlexicalAnalizator/input.js");
    if (!file.is_open()) {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    std::string jsCode((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

    lexRegexAnalyze(jsCode);

    return 0;
}

