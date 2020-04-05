#include <iostream>
#include "../../src/thread/Thread.h"

void fun1()
{
    std::cout << "test1: zh::Thread th(fun1)" << std::endl;
}

void fun2(int a, int b)
{
    std::cout << "test2: a = " << a << ", b = " << b << ", a + b = " << (a + b) << std::endl;
}

void test1()
{
    zh::Thread th(fun1);
    th.join();
}

void test2()
{
    zh::Thread th(fun2, 1, 2);
    th.join();
}

int main()
{
    test1();
    test2();

    return 0;
}