#include "b.h"
#include <iostream>
using namespace std;

int main()
{
    B *b = new B;
    b->a = new A;
    delete b;
}

/**
 * OUTPUT:
 *  B construction
 *  A constructs a array of 5 int
 *  A destructs and release space of tmp
 *  B destruction
 * **************************************
 *
 * the most fun part is compilation.
 *
 * I took 2 ways to compile multiple source code 
 *
 * 1. (easy) g++ -o xxx main.cpp a.cpp b.cpp
 *
 * 2. (more complicate) 
 *  1) g++ -c a.cpp -o a.o
 *  2) g++ -c b.cpp -o b.o
 *  3) g++ main.cpp -o main  a.o b.o 
 *
 * More gcc/g++ arguments to be learned !!!!!
 */
