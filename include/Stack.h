
#ifndef LOX_INTERPRETER_STACK_H
#define LOX_INTERPRETER_STACK_H
// http://www.cplusplus.com/doc/oldtutorial/templates/
// Non-type parameters for templates
#include <assert.h>
#include <iostream>

template <typename T, int N>
class Stack {
private:
    T _stack[N];
    int top;
public:
    Stack(): top(0) {};
    int size();
    T pop_back();
    void push_back(T value);
    T back();
    T & operator[](int index);
    T & indexFromEnd(int index);
};

template<typename T, int N>
int Stack<T, N>::size() {
    return top;
}

template<typename T, int N>
T Stack<T, N>::pop_back() {
    std::cerr << "pop_back()" << std::endl;
    assert(top > 0 && "pop_back from empty stack");
    return _stack[--top];
}

template<typename T, int N>
void Stack<T, N>::push_back(T value) {
    std::cerr << "push_back()" << std::endl;
    assert(top < N && "push_back to full stack");
    _stack[top++] = value;
}

template<typename T, int N>
T Stack<T, N>::back() {
    std::cerr << "back()" << std::endl;
    assert(top > 0 && "back() for empty stack");
    return _stack[top - 1];
}

template<typename T, int N>
T &Stack<T, N>::operator[](int index) {
    std::cerr << "operator[]" << std::endl;
    std::cerr << "index: " << index << "top: " << top << std::endl;
    assert((index >= 0 && index < top) && "[]: index out of stack range" );
    return _stack[index];

}

template<typename T, int N>
T &Stack<T, N>::indexFromEnd(int endIndex) {
    // 0: element on _stack top
    std::cerr << "indexFromEnd()" << std::endl;
    int index = top - 1 - endIndex;
    assert((index >= 0 && index < top) && "indexfromEnd: index out of stack range");
    return _stack[index];
}


#endif //LOX_INTERPRETER_STACK_H
