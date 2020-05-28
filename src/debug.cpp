
#include <debug.h>
#include <Chunk.h>
#include <iostream>

std::vector<std::string> tokentype_to_string  = {
        "TOKEN_LEFT_PAREN", "TOKEN_RIGHT_PAREN",
        "TOKEN_LEFT_BRACE", "TOKEN_RIGHT_BRACE",
        "TOKEN_COMMA", "TOKEN_DOT", "TOKEN_MINUS", "TOKEN_PLUS",
        "TOKEN_SEMICOLON", "TOKEN_SLASH", "TOKEN_STAR",

        // One or two character tokens.
        "TOKEN_BANG", "TOKEN_BANG_EQUAL",
        "TOKEN_EQUAL", "TOKEN_EQUAL_EQUAL",
        "TOKEN_GREATER", "TOKEN_GREATER_EQUAL",
        "TOKEN_LESS", "TOKEN_LESS_EQUAL",

        // Literals.
        "TOKEN_IDENTIFIER", "TOKEN_STRING", "TOKEN_NUMBER",

        // Comments
        "TOKEN_COMMENT",

        // Keywords.
        "TOKEN_AND", "TOKEN_CLASS", "TOKEN_ELSE", "TOKEN_FALSE",
        "TOKEN_FOR", "TOKEN_FUN", "TOKEN_IF", "TOKEN_NIL", "TOKEN_OR",
        "TOKEN_PRINT", "TOKEN_RETURN", "TOKEN_SUPER", "TOKEN_THIS",
        "TOKEN_TRUE", "TOKEN_VAR", "TOKEN_WHILE",

        "TOKEN_ERROR",
        "TOKEN_EOF"
};

std::vector<std::string> opcode_to_string = {
//> op-constant
        "OP_CONSTANT",
//< op-constant
//> Types of Values literal-ops
        "OP_NIL",
        "OP_TRUE",
        "OP_FALSE",
//< Types of Values literal-ops
//> Global Variables pop-op
        "OP_POP",
//< Global Variables pop-op
//> Local Variables get-local-op
        "OP_GET_LOCAL",
//< Local Variables get-local-op
//> Local Variables set-local-op
        "OP_SET_LOCAL",
//< Local Variables set-local-op
//> Global Variables get-global-op
        "OP_GET_GLOBAL",
//< Global Variables get-global-op
//> Global Variables define-global-op
        "OP_DEFINE_GLOBAL",
//< Global Variables define-global-op
//> Global Variables set-global-op
        "OP_SET_GLOBAL",
//< Global Variables set-global-op
//> Closures upvalue-ops
        "OP_GET_UPVALUE",
        "OP_SET_UPVALUE",
//< Closures upvalue-ops
//> Classes and Instances property-ops
        "OP_GET_PROPERTY",
        "OP_SET_PROPERTY",
//< Classes and Instances property-ops
//> Superclasses get-super-op
        "OP_GET_SUPER",
//< Superclasses get-super-op
//> Types of Values comparison-ops
        "OP_EQUAL",
        "OP_GREATER",
        "OP_LESS",
//< Types of Values comparison-ops
//> A Virtual Machine binary-ops
        "OP_ADD",
        "OP_SUBTRACT",
        "OP_MULTIPLY",
        "OP_DIVIDE",
//> Types of Values not-op
        "OP_NOT",
//< Types of Values not-op
//< A Virtual Machine binary-ops
//> A Virtual Machine negate-op
        "OP_NEGATE",
//< A Virtual Machine negate-op
//> Global Variables op-print
        "OP_PRINT",
//< Global Variables op-print
//> Jumping Back and Forth jump-op
        "OP_JUMP",
//< Jumping Back and Forth jump-op
//> Jumping Back and Forth jump-if-false-op
        "OP_JUMP_IF_FALSE",
//< Jumping Back and Forth jump-if-false-op
//> Jumping Back and Forth loop-op
        "OP_LOOP",
//< Jumping Back and Forth loop-op
//> Calls and Functions op-call
        "OP_CALL",
//< Calls and Functions op-call
//> Methods and Initializers invoke-op
        "OP_INVOKE",
//< Methods and Initializers invoke-op
//> Superclasses super-invoke-op
        "OP_SUPER_INVOKE",
//< Superclasses super-invoke-op
//> Closures closure-op
        "OP_CLOSURE",
//< Closures closure-op
//> Closures close-upvalue-op
        "OP_CLOSE_UPVALUE",
//< Closures close-upvalue-op
        "OP_RETURN",
//> Classes and Instances class-op
        "OP_CLASS",
//< Classes and Instances class-op
//> Superclasses inherit-op
        "OP_INHERIT",
//< Superclasses inherit-op
//> Methods and Initializers method-op
        "OP_METHOD"
//< Methods and Initializers method-op
};

void disassembleChunk(Chunk* chunk) {
    for(int i = 0; i < chunk->code.size(); ) {
        i = disassembleInstruction(chunk, i);
    }
}

int disassembleInstruction(Chunk* chunk, int i) {
    auto opCode = chunk->code[i];
    switch (static_cast<OpCode>(opCode)) {
        case OpCode::OP_ADD:
        case OpCode::OP_SUBTRACT:
        case OpCode::OP_MULTIPLY:
        case OpCode::OP_DIVIDE:
        case OpCode::OP_NEGATE:
            std::cout << opcode_to_string[opCode] << std::endl;
            return i + 1;
        case OpCode::OP_CONSTANT:
            Value value = chunk->constants[chunk->code[i+1]];
            std::cout << opcode_to_string[opCode] << " ";
            if(auto pd = std::get_if<double>(&value)) {
                std::cout << std::to_string(*pd);
            } else {
                std::cout << "constant of unsupported type";
            }
            std::cout << std::endl;
            return i + 2;
    }
    throw std::logic_error("unhandled case in debug");
}