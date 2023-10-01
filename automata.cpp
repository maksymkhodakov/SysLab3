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
    ERROR
};

enum Symbol {
    LETTER,
    DIGIT,
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
        {{NUMERIC_LITERAL, DIGIT}, NUMERIC_LITERAL},
        {{KEYWORD_OR_IDENTIFIER, WHITESPACE}, FINAL},
        {{NUMERIC_LITERAL, WHITESPACE}, FINAL},
        {{KEYWORD_OR_IDENTIFIER, WHITESPACE}, FINAL},
        {{NUMERIC_LITERAL, WHITESPACE}, FINAL},
        {{OPERATOR, WHITESPACE}, FINAL},
        {{PUNCTUATION, WHITESPACE}, FINAL},
};

Symbol getSymbol(char ch) {
    if (std::isalpha(ch)) return LETTER;
    if (std::isdigit(ch)) return DIGIT;
    if (std::isspace(ch)) return WHITESPACE;
    if (strchr("+-*/=<>!&|", ch)) return OPERATOR_SYM;
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
            std::cout << currentLexeme << " - ";
            if (currentLexeme == "function" || currentLexeme == "var" ||
                currentLexeme == "return" || currentLexeme == "if" ||
                currentLexeme == "else" || currentLexeme == "while" ||
                currentLexeme == "for") {
                std::cout << "Keyword\n";
            } else if (currentState == NUMERIC_LITERAL) {
                std::cout << "Numeric Literal\n";
            } else {
                std::cout << "Identifier\n";
            }
            currentLexeme.clear();
            currentState = START;
        } else if (nextState == ERROR) {
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
    std::ifstream file("/Users/maksymkhodakov/CLionProjects/JSlexicalAnalizator/input.js");
    if (!file.is_open()) {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    std::string jsCode((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

    lexAutomataAnalyze(jsCode);

    return 0;
}
