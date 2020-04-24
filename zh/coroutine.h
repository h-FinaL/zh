#ifndef __ZH_COROUTINE_H__
#define __ZH_COROUTINE_H__

#include <ucontext.h>
#include <stack>
#include <memory>
#include <stdlib.h>

namespace zh
{
#define CO_STACK_SIZE 128 * 1024

typedef void (*callback)(void*);

class CoroutineEnv;
class Coroutine
{
public:

    Coroutine(callback pfn, void* args, int stackSize = CO_STACK_SIZE);
    ~Coroutine();
    //把目标协程与当前正在执行的协程交换
    void resume();
    //协程让出使用权，切换到上一个协程交换
    void yield();
    void reset();

private:
    Coroutine();

public:
    static void yieldCurrEnv();
    static void coroutineFunc();
    static CoroutineEnv* getCurrEnv();
    static Coroutine* getCurrCo();

private:
    friend CoroutineEnv;
    callback m_pfn;
    void* m_args;

    int m_stackSize;
    void* m_stack;

    int m_isMain;
    ucontext_t m_context;
};

class CoroutineEnv
{
private:
    CoroutineEnv();
    void addCurrCo(Coroutine* coroutine);
    void yieldCurrCo();
    std::stack<Coroutine*> pCoStack;
    
    friend Coroutine;
};
    
}

#endif