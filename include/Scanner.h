#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include <Token.h>
#include <unordered_map>

class Scanner {
private:
    static const std::unordered_map<std::string, TokenType> keywords;
    std::string source;
    int start;
    int current;
    int line;
    bool hasError;
public:
    Scanner(std::string source):
        source(std::move(source)), start(0), current(0), line(1), hasError(false) {

    }

    std::vector<Token> scanTokens();
    Token scanToken();
    char getChar();
    bool isAtEnd();
    Token makeToken(TokenType type);
    bool match(char ch);
    void skipWhiteSpace();
    char peekCurrent();
    Token makeDigit();
    Token makeString();
    bool error() {
        return hasError;
    }
    std::string getIdentifier();
};

#endif