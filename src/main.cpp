#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <sstream>
#include <Token.h>
#include <Scanner.h>
#include <Parser.h>
#include <CodeGenerator.h>
#include <VM.h>
#include <GarbageCollector.h>

#include <common.h>
#ifdef LOX_DEBUG
#include <AstPrinter.h>
#include <debug.h>
#endif

#ifdef LOX_DEBUG
void run(std::string source, bool exit_on_error) {
    Scanner scanner(std::move(source));
    auto tokens = scanner.scanTokens();
    if(scanner.error()) {
        auto errorToken = tokens.at(0);
        std::cout << "Error at line " << errorToken.line << ": "
                  << errorToken.lexeme << std::endl;
        if(exit_on_error) {
            return;
        }
    } else {
        for(const auto & t: tokens) {
            std::cout << "lexeme: " <<  t.lexeme <<
                      " with type: " << tokentype_to_string[t.type] <<
                      std::endl;
        }
        auto parser = Parser(tokens);
        auto expr = parser.parse();
        AstPrinter astPrinter;
        astPrinter.printAst(*expr);
        std::cout << std::endl;
        CodeGenerator codeGenerator;
        codeGenerator.compile(*expr);
        disassembleChunk(&codeGenerator.chunk);
        VM vm(codeGenerator.chunk);
        vm.run();
        auto other = vm;  // breakpoint here: check vm.stack
    }
}
#else
void run(std::string source, bool exit_on_error) {
    Scanner scanner(std::move(source));
    auto tokens = scanner.scanTokens();
    if(scanner.error()) {
        auto errorToken = tokens.at(0);
        std::cout << "Error at line " << errorToken.line << ": "
                  << errorToken.lexeme << std::endl;
        if(exit_on_error) {
            return;
        }
    } else {
        auto parser = Parser(tokens);
        parser.parse();
        CodeGenerator codeGenerator;
        auto functionObj = codeGenerator.compile(parser.statements);
        VM vm;
        vm.start(functionObj);
//        auto other = vm;  // breakpoint here: check vm.stack
//        GarbageCollector::getInstance().freeAllObjects();

    }

}
#endif

void runLine() {
    std::string source;
    while(true) {
        std::cout << "> ";
        std::getline(std::cin, source);
        if(source == "quit") {
            break;
        }
        run(source, false);
    }
}

void runFile(char * filepath) {
    std::ifstream source_file(filepath);
    std::stringstream buffer;
    buffer << source_file.rdbuf();
    run(buffer.str(), true);
}

int main(int argc, char *argv[])
{
    if(argc == 1) {
        runLine();
    } else if(argc == 2){
        runFile(argv[1]);
    }
    return 0;
}