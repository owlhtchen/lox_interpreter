#include <Scanner.h>
#include <cctype>
#include <iostream>

const std::unordered_map<std::string, TokenType>  Scanner::keywords({
    {"and", TOKEN_AND},
    {"class", TOKEN_CLASS},
    {"else", TOKEN_ELSE},
    {"false", TOKEN_FALSE},
    {"for", TOKEN_FOR},
    {"fun", TOKEN_FUN},
    {"if", TOKEN_IF},
    {"nil", TOKEN_NIL},
    {"or", TOKEN_OR},
    {"print", TOKEN_PRINT},
    {"return", TOKEN_RETURN},
    {"super", TOKEN_SUPER},
    {"this", TOKEN_THIS},
    {"true", TOKEN_TRUE},
    {"var", TOKEN_VAR},
    {"while", TOKEN_WHILE}
});

std::vector<Token> Scanner::scanTokens() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        auto token = scanToken();
        if(token.type == TOKEN_COMMENT) {
            continue;
        } else if(token.type == TOKEN_ERROR) {
            hasError = true;
            return std::vector<Token>{token};
        }
        tokens.push_back(token);
    }
    return tokens;
}

void Scanner::skipWhiteSpace() {
    while(!isAtEnd() && isspace(source[current])) {
        current++;
    }
    start = current;
}

Token Scanner::scanToken() {
    skipWhiteSpace();
    char ch = getChar();
    if(isAtEnd()) {
        return makeToken(TOKEN_EOF);
    }
    switch (ch) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case '/': {
            if(match('/')) {
                // skip comment
                while(peekCurrent() != '\n' || peekCurrent() != -1) {
                }
                return makeToken(TOKEN_COMMENT);
            } else {
                return makeToken(TOKEN_SLASH);
            }
        }
        case '*': return makeToken(TOKEN_STAR);
        case '!': return makeToken(
                match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=': return makeToken(
                match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '>': return makeToken(
                match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '<': return makeToken(
                match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        default:
            if(isdigit(ch)) {
                return makeDigit();
            } else if(ch == '"') {
                return makeString();
            } else if(isalpha(ch)){
                // identifier or keyword
                auto word = getIdentifier();
                auto iter = Scanner::keywords.find(word);
                if(iter == Scanner::keywords.end()) {
                    // identifier
                    return Token(TOKEN_IDENTIFIER,
                            word,
                            line);
                } else {
                    // identifier
                    return Token(iter->second, iter->first, line);
                }
            }
    }
    throw "unexpected token in scanner";
}

bool isalnum_(char ch) {
    return isalnum(ch) || ch == '_';
}

std::string Scanner::getIdentifier() {
    while (isalnum_(peekCurrent())) {
        current++;
    }
    return source.substr(start, current - start);
}

char Scanner::peekCurrent() {
    if(isAtEnd()) {
        return -1;
    }
    return source[current];
}

Token Scanner::makeDigit() {
    while (isdigit(peekCurrent())) {
        current++;
    }
    if(peekCurrent() == '.') {
        while (isdigit(peekCurrent())) {
            current++;
        }
    }
    return makeToken(TOKEN_NUMBER);
}

Token Scanner::makeString() {
    while(peekCurrent() != -1 && peekCurrent() != '"') {
        current++;
    }
    if(isAtEnd()) {
        return Token(TOKEN_ERROR, "not terminated string", line);
    }
    return Token(TOKEN_STRING, source.substr(start + 1, current - 1) , line);
}

Token Scanner::makeToken(TokenType type) {
    return Token(type, source.substr(start, current - start), line);
}

char Scanner::getChar() {
    return source[current++];
}

bool Scanner::isAtEnd() {
    return current >= source.length();
}

bool Scanner::match(char ch) {
    if(isAtEnd()) return false;
    if(source[current] != ch) return false;
    current++;
    return true;
}