#ifndef __ZH_COROUTINE_H__
#define __ZH_COROUTINE_H__

#include <ucontext.h>

namespace zh
{
#define CO_STACK_SIZE 128 * 1024
#define uint unsigned int

static __thread Coroutine* t_coroutine;

typedef void (*callback)();

class Coroutine
{
public:

    Coroutine(callback pfn, uint stackSize = CO_STACK_SIZE);
    ~Coroutine();
    void resume();
    void yield();
    void reset();
    void reset();

private:
    Coroutine();

public:
    static Coroutine* getThis();
    static void mainFun();
    static void setThis(Coroutine* Coroutine);
    static void yield();

private:
    callback m_pfn;
    uint m_stackSize;
    void* m_stack;
    ucontext_t m_context;
};

    
}

#endif