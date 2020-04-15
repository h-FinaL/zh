#include "coroutine.h"

namespace zh
{

static __thread ThrCoroutine* thrCoroutine;

Coroutine::Coroutine() :
    m_pfn(nullptr),
    m_args(nullptr),
    m_stackSize(0),
    m_stack(nullptr)
{
}

Coroutine::Coroutine(callback pfn, void* args, int stackSize = CO_STACK_SIZE) :
    m_pfn(pfn),
    m_args(args)
{
    m_stackSize = stackSize ? stackSize : CO_STACK_SIZE;
    m_stack = malloc(m_stackSize);

    getcontext(&m_context);
    m_context.uc_link = nullptr;
    m_context.uc_stack.ss_sp = m_stack;
    m_context.uc_stack.ss_size = m_stackSize;
}

void Coroutine::resume()
{

}

ThrCoroutine* Coroutine::getCurrThr()
{
    if (thrCoroutine)
        return thrCoroutine;
    
    thrCoroutine = new ThrCoroutine();
    return thrCoroutine;
}

ThrCoroutine::ThrCoroutine()
{
    Coroutine* coroutine = new Coroutine();
    pCoStack.push(coroutine);
}

}