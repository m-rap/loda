//
// Created by Rian Prakoso on 7/10/20.
//

#include <stdio.h>

class HelloPrinter {
public:
    void print() {
        printf("Hello from HelloPrinter\n");
    }
};

extern "C" void execute() {
    HelloPrinter helloPrinter;
    helloPrinter.print();
}