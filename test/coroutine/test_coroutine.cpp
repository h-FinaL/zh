#include <iostream>
#include <pthread.h>
#include "../../coroutine.h"

void fun2(void*);

void fun1(void*)
{
    std::cout << "aaa" << std::endl;
    std::cout << "bbb" << std::endl;
    zh::Coroutine co2(fun2, NULL);
    co2.resume();
    std::cout << "ccc" << std::endl;
    std::cout << "ddd" << std::endl;
    co2.resume();
    std::cout << "eee" << std::endl;
}

void fun2(void*)
{
    std::cout << "111" << std::endl;
    std::cout << "222" << std::endl;
    zh::Coroutine::yield_curr_env();
    std::cout << "333" << std::endl;
    std::cout << "444" << std::endl;
}

void* fun(void *)
{
    zh::Coroutine co1(fun1, NULL);
    co1.resume();
}

int main()
{
    pthread_t id;
    pthread_create(&id, NULL, fun, NULL);
    pthread_join(id, NULL);


    return 0;
}