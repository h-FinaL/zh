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
    enum State {
        /// 初始化状态
        INIT,
        /// 暂停状态
        HOLD,
        /// 执行中状态
        EXEC,
        /// 结束状态
        TERM,
        /// 可执行状态
        READY,
        /// 异常状态
        EXCEPT
    };
public:

    Coroutine(callback pfn, void* args, int stackSize = CO_STACK_SIZE);
    ~Coroutine();
    void resume();
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