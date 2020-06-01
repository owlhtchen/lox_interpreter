# lox_interpreter

Two pass interpreter for toy programming language lox

source code -> AST -> bytecode -> vm(with gc)


visitor pattern for code generator when traversing AST tree

singleton pattern for garbage collector (https://stackoverflow.com/questions/86582/singleton-how-should-it-be-used), 
and all objects are stored and tracked in GarbageCollector class (singleton)

I used std::monostate() in Value to represent nil in lox