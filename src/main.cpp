#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <sstream>
#include <Token.h>
#include <Scanner.h>

void run(std::string source) {
    Scanner scanner(std::move(source));
    auto tokens = scanner.scanTokens();
    if(scanner.error()) {
        auto errorToken = tokens.at(0);
        std::cout << "Error at line " << errorToken.line << ": "
        << errorToken.lexeme << std::endl;
    } else {
        for(const auto & t: tokens) {
            std::cout << "lexeme: " <<  t.lexeme << std::endl;
        }
    }
}

void runLine() {
    std::string source;
    while(true) {
        std::cout << "> ";
        std::getline(std::cin, source);
        if(source == "quit") {
            break;
        }
        run(source);
    }
}

void runFile(char * filepath) {
    std::ifstream source_file(filepath);
    std::stringstream buffer;
    buffer << source_file.rdbuf();
    run(buffer.str());
}

int main(int argc, char *argv[])
{
    
//    Token t(TOKEN_BANG, "!", 100);
//    std::cout <<  t.lexeme << " " << t.line << std::endl;
    if(argc == 1) {
        runLine();
    } else if(argc == 2){
        runFile(argv[1]);
    }
    return 0;
}