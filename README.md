# lox_interpreter

Two pass interpreter for toy programming language lox

source code -> AST -> bytecode -> vm(with gc)


visitor pattern for code generator when traversing AST tree

singleton pattern for garbage collector (https://stackoverflow.com/questions/86582/singleton-how-should-it-be-used), 
and all objects are stored and tracked in GarbageCollector class (singleton)

I used std::monostate() in Value to represent nil in lox

locals and upValues in FunctionCompiler (during compiling) "emulates" the location(index) of local variables in 
&vm.stack[stackBase] and UpValueObj in ClosureObj during runtime, and this location is used as operands for opCodes,
e.g. OP_GET_LOCAL, OP_GET_UPVALUE
