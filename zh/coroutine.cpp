#include "coroutine.h"

namespace zh
{

static __thread CoroutineEnv* coroutineEnv;

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
    makecontext(&m_context, coroutineFunc, 0);
}

void Coroutine::coroutineFunc()
{
    Coroutine* currCo = getCurrCo();
    if (currCo->m_pfn)
        currCo->m_pfn(currCo->m_args);
    
}

void Coroutine::resume()
{
    Coroutine* currCo = getCurrCo();
    getCurrEnv()->addCurrCo(this);
    swapcontext(&currCo->m_context, &this->m_context);
}

void Coroutine::yield()
{
    getCurrEnv()->yieldCurrCo();
}

void Coroutine::yieldCurrEnv()
{
    getCurrEnv()->yieldCurrCo();
}

Coroutine* Coroutine::getCurrCo()
{
    return getCurrEnv()->pCoStack.top();
}

CoroutineEnv* Coroutine::getCurrEnv()
{
    if (coroutineEnv)
        return coroutineEnv;
    
    coroutineEnv = new CoroutineEnv();
    return coroutineEnv;
}

CoroutineEnv::CoroutineEnv()
{
    Coroutine* coroutine = new Coroutine();
    pCoStack.push(coroutine);
}

void CoroutineEnv::yieldCurrCo()
{
    Coroutine* currCo = pCoStack.top();
    pCoStack.pop();
    Coroutine* lastCo = pCoStack.top();
    swapcontext(&currCo->m_context, &lastCo->m_context);
}

void CoroutineEnv::addCurrCo(Coroutine* coroutine)
{
    pCoStack.push(coroutine);
}

}