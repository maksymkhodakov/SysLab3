#include <iostream>
#include <regex>
#include <fstream>

void lexRegexAnalyze(const std::string &jsCode, std::ofstream &outfile) {
    std::regex whitespaceRegex("(\\s+)");
    std::regex keywordRegex("\\b(function|var|return|if|else|while|for)\\b");
    std::regex identifierRegex("\\b([a-zA-Z_][a-zA-Z0-9_]*)\\b");
    std::regex dotRegex("\\.");
    std::regex numericLiteralRegex("(\\b\\d+\\.?\\d*([eE][-+]?\\d+)?\\b|\\b0x[0-9a-fA-F]+\\b)");
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
                    if (type != "Whitespace") {
                        outfile << match[0] << " - " << type << "\n";
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
    int retryLimit = 3;
    int attempts = 0;

    std::string directory = "/Users/maksymkhodakov/CLionProjects/JSlexicalAnalizator/";
    std::string filename;

    std::ifstream file;

    do {
        if (attempts > 0) {
            std::cout << "Would you like to retry? (yes/no): ";
            std::string choice;
            std::cin >> choice;
            if (choice != "yes") {
                std::cerr << "Exiting program.\n";
                return 1;
            }
        }

        std::cout << "Enter a filename: ";
        std::cin >> filename;

        std::string fullPath = directory + filename;
        file.open(fullPath);

        if (file.is_open()) {
            break;
        }

        std::cerr << "Failed to open file '" << fullPath << "'.\n";
        std::cerr << "Error: " << std::strerror(errno) << std::endl;
        attempts++;

    } while (attempts < retryLimit);

    if (!file.is_open()) {
        std::cerr << "Exceeded maximum retries. Exiting program.\n";
        return 1;
    }

    std::string jsCode((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

    std::ofstream outfile("/Users/maksymkhodakov/CLionProjects/JSlexicalAnalizator/output.txt");

    lexRegexAnalyze(jsCode, outfile);

    outfile.close();

    return 0;
}
