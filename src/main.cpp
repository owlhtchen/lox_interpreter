#include <iostream>
#include <Scanner.h>

int main(int argc, char *argv[])
{
    
    Token t(TOKEN_BANG, "!", 100);
    std::cout <<  t.lexeme << " " << t.line << std::endl;
    return 0;
}