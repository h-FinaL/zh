#include "coroutine.h"

namespace zh
{

static __thread CoroutineEnv* coroutineEnv;

Coroutine::Coroutine() :
    m_pfn(nullptr),
    m_args(nullptr),
    m_stackSize(0),
    m_stack(nullptr),
    m_isMain(1)
{
}

Coroutine::~Coroutine()
{
    if (m_isMain == 0 && m_stack != nullptr)
        free(m_stack);
}

Coroutine::Coroutine(callback pfn, void* args, int stackSize) :
    m_pfn(pfn),
    m_args(args),
    m_isMain(0)
{
    m_stackSize = stackSize ? stackSize : CO_STACK_SIZE;
    m_stack = malloc(m_stackSize);

    getcontext(&m_context);
    m_context.uc_link = nullptr;
    m_context.uc_stack.ss_sp = m_stack;
    m_context.uc_stack.ss_size = m_stackSize;
    makecontext(&m_context, coroutine_func, 0);
}

void Coroutine::coroutine_func()
{
    Coroutine* currCo = get_curr_co();
    if (currCo->m_pfn)
        currCo->m_pfn(currCo->m_args);
    yield_curr_env();
}

void Coroutine::resume()
{
    Coroutine* currCo = get_curr_co();
    get_curr_env()->add_curr_co(this);
    swapcontext(&currCo->m_context, &this->m_context);
}

void Coroutine::yield()
{
    get_curr_env()->yield_curr_co();
}

void Coroutine::yield_curr_env()
{
    get_curr_env()->yield_curr_co();
}

Coroutine* Coroutine::get_curr_co()
{
    return get_curr_env()->p_co_stack.top();
}

CoroutineEnv* Coroutine::get_curr_env()
{
    if (coroutineEnv)
        return coroutineEnv;
    
    coroutineEnv = new CoroutineEnv();
    return coroutineEnv;
}

CoroutineEnv::CoroutineEnv()
{
    Coroutine* coroutine = new Coroutine();
    p_co_stack.push(coroutine);
}

void CoroutineEnv::yield_curr_co()
{
    Coroutine* currCo = p_co_stack.top();
    if (currCo->m_isMain == 0)
    {
        p_co_stack.pop();
        Coroutine* lastCo = p_co_stack.top();
        swapcontext(&currCo->m_context, &lastCo->m_context);
    }
}

void CoroutineEnv::add_curr_co(Coroutine* coroutine)
{
    p_co_stack.push(coroutine);
}

}