#include <iostream>
#include <unordered_map>
#include <vector>
#include <cctype>
#include <fstream>

enum State {
    START,
    KEYWORD_OR_IDENTIFIER,
    NUMERIC_LITERAL,
    OPERATOR,
    PUNCTUATION,
    FINAL,
    ERROR,
    IN_STRING_DOUBLE,
    IN_STRING_SINGLE,
    IN_COMMENT_SINGLE,
    IN_COMMENT_MULTI_START,
    IN_COMMENT_MULTI_END
};

enum Symbol {
    LETTER,
    DIGIT,
    DOUBLE_QUOTE,
    SINGLE_QUOTE,
    SLASH,
    STAR,
    WHITESPACE,
    OPERATOR_SYM,
    PUNCTUATION_SYM,
    OTHER
};

struct Transition {
    State state;
    Symbol symbol;

    bool operator==(const Transition &other) const {
        return state == other.state && symbol == other.symbol;
    }
};

namespace std {
    template <>
    struct hash<Transition> {
        std::size_t operator()(const Transition& transition) const {
            return std::hash<int>()(transition.state) ^ (std::hash<int>()(transition.symbol) << 1);
        }
    };
}

std::unordered_map<Transition, State> transitionTable = {
        {{START, LETTER}, KEYWORD_OR_IDENTIFIER},
        {{START, DIGIT}, NUMERIC_LITERAL},
        {{KEYWORD_OR_IDENTIFIER, LETTER}, KEYWORD_OR_IDENTIFIER},
        {{KEYWORD_OR_IDENTIFIER, DIGIT}, KEYWORD_OR_IDENTIFIER},
        {{KEYWORD_OR_IDENTIFIER, WHITESPACE}, FINAL},
        {{NUMERIC_LITERAL, DIGIT}, NUMERIC_LITERAL},
        {{NUMERIC_LITERAL, WHITESPACE}, FINAL},
        {{START, DOUBLE_QUOTE}, IN_STRING_DOUBLE},
        {{IN_STRING_DOUBLE, DOUBLE_QUOTE}, FINAL},
        {{START, SINGLE_QUOTE}, IN_STRING_SINGLE},
        {{IN_STRING_SINGLE, SINGLE_QUOTE}, FINAL},
        {{START, SLASH}, IN_COMMENT_SINGLE},
        {{IN_COMMENT_SINGLE, SLASH}, IN_COMMENT_MULTI_START},
        {{IN_COMMENT_MULTI_START, STAR}, IN_COMMENT_MULTI_END},
        {{IN_COMMENT_MULTI_END, SLASH}, FINAL},
        {{START, OPERATOR_SYM}, OPERATOR},
        {{OPERATOR, WHITESPACE}, FINAL},
        {{START, PUNCTUATION_SYM}, PUNCTUATION},
        {{PUNCTUATION, WHITESPACE}, FINAL},
};

Symbol getSymbol(char ch) {
    if (std::isalpha(ch)) return LETTER;
    if (std::isdigit(ch)) return DIGIT;
    if (ch == '"') return DOUBLE_QUOTE;
    if (ch == '\'') return SINGLE_QUOTE;
    if (ch == '/') return SLASH;
    if (ch == '*') return STAR;
    if (std::isspace(ch)) return WHITESPACE;
    if (strchr("+-*/=<>!&|%^~", ch)) return OPERATOR_SYM;
    if (strchr("(){}[];,.", ch)) return PUNCTUATION_SYM;
    return OTHER;
}

void lexAutomataAnalyze(const std::string &jsCode) {
    State currentState = START;
    std::string currentLexeme;

    for (char ch : jsCode) {
        Symbol symbol = getSymbol(ch);
        Transition transition = {currentState, symbol};
        auto it = transitionTable.find(transition);
        State nextState = it != transitionTable.end() ? it->second : ERROR;

        if (nextState == FINAL) {
            if (currentState == IN_STRING_DOUBLE || currentState == IN_STRING_SINGLE) {
                std::cout << currentLexeme << " - String Literal\n";
            } else if (currentState == IN_COMMENT_MULTI_END) {
                std::cout << currentLexeme << " - Multi-line Comment\n";
            } else if (currentState == IN_COMMENT_SINGLE) {
                std::cout << currentLexeme << " - Single-line Comment\n";
            } else if (currentState == KEYWORD_OR_IDENTIFIER) {
                if (currentLexeme == "function" || currentLexeme == "var" ||
                    currentLexeme == "return" || currentLexeme == "if" ||
                    currentLexeme == "else" || currentLexeme == "while" ||
                    currentLexeme == "for") {
                    std::cout << currentLexeme << " - Keyword\n";
                } else {
                    std::cout << currentLexeme << " - Identifier\n";
                }
            } else if (currentState == NUMERIC_LITERAL) {
                std::cout << currentLexeme << " - Numeric Literal\n";
            } else if (currentState == OPERATOR) {
                std::cout << currentLexeme << " - Operator\n";
            } else if (currentState == PUNCTUATION) {
                std::cout << currentLexeme << " - Punctuation\n";
            }
            currentLexeme.clear();
            currentState = START;
        } else if (nextState == ERROR && !currentLexeme.empty()) {
            std::cerr << "Unrecognized sequence: " << currentLexeme << ch << '\n';
            currentLexeme.clear();
            currentState = START;
        } else {
            currentLexeme += ch;
            currentState = nextState;
        }
    }
}

int main() {
    int retryLimit = 3;
    int attempts = 0;

    std::string directory = "../";
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

    lexAutomataAnalyze(jsCode);

    return 0;
}
