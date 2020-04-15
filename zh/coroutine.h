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

class ThrCoroutine;
class Coroutine
{
public:

    Coroutine(callback pfn, void* args, int stackSize = CO_STACK_SIZE);
    ~Coroutine();
    void resume();
    void yield();
    void reset();
    void reset();

private:
    Coroutine();

public:
    static void yieldCurrThr();
    static void coroutineFunc(Coroutine* coroutine);
    static ThrCoroutine* getCurrThr();

private:
    friend ThrCoroutine;
    callback m_pfn;
    void* m_args;

    int m_stackSize;
    void* m_stack;

    ucontext_t m_context;
};

class ThrCoroutine
{
private:
    ThrCoroutine();
    friend Coroutine;
    std::stack<Coroutine*> pCoStack;
};
    
}

#endif