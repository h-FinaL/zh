#ifndef __ZH_COROUTINE_H__
#define __ZH_COROUTINE_H__

#include <ucontext.h>

namespace zh
{
#define CO_STACK_SIZE 128 * 1024
#define uint unsigned int

typedef void (*callback)(void*);

class Coroutine
{
public:

    Coroutine(callback pfn, uint stackSize = CO_STACK_SIZE);
    ~Coroutine();
    void reset();
    void swapIn();
    void swapOut();

public:
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