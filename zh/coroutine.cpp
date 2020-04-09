#include "coroutine.h"

namespace zh
{

Coroutine::Coroutine(callback pfn, uint stackSize) :
    m_pfn(pfn)
{
    m_stackSize = stackSize ? stackSize : CO_STACK_SIZE;
    m_stack = new char[stackSize];

    getcontext(&m_context);
    m_context.uc_link = nullptr;
    m_context.uc_stack.ss_sp = m_stack;
    m_context.uc_stack.ss_size = m_stackSize;


}


}