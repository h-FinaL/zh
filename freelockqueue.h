#pragma once

#include <atomic>

template <typename T>
class freelockqueue
{
    enum class retype
    {
        enqueue_chunk_complate,
        enqueue_chunk_new,
        dequeue_chunk_free,
        dequeue_chunk_empty,
        dequeue_chunk_complate
    };

    typedef unsigned int index_t;

    struct chunk
    {
        retype enqueue(T&& value)
        {
            index_t tail_index = _tail_index.load();
            do
            {
                if (tail_index >= 32)
                    return enqueue_chunk_new;
            } while (!tail_index.compare_exchange_weak(p, p + 1));
            
            _value[tail_index] = std::move(value);

            return enqueue_chunk_complate;
        }

        retype dequeue(T& value)
        {
            index_t head_index;
            index_t tail_index;

            head_index = _head_index.load();
            tail_index = _tail_index.load();

            if (tail_index >= 32)
                return dequeue_chunk_free;

            if (head_index >= tail_index)
                return dequeue_chunk_empty;
            
            value = std::move(_value[head_index]);
            return dequeue_chunk_complate;
        }

        T _value[32];
        std::atomic<index_t> _head_index{ 0 };
        std::atomic<index_t> _tail_index{ 0 };
        chunk* _next{ nullptr };
    };


public:
    freelockqueue(){}
    ~freelockqueue(){}

    void enqueue(T&& vaule)
    {
        while (_head_chunk->enqueue(std::forward(vaule) == enqueue_chunk_new)
        {
            bool new_chunk = false;
            if (_new_chunk.compare_exchange_strong(new_chunk, true))
            {
                if (_head_chunk->enqueue(std::forward(vaule)) == enqueue_chunk_new)
                {
                    new_chunk();
                    _head_chunk->enqueue(std::forward(vaule));
                    _new_chunk.exchange(false);
                    return;
                }
            }
        }
    }

    void dequeue(T& vaule)
    {

    }

private:
    void new_chunk()
    {
        chunk* ck = new chunk;
        _tail_chunk->_next = ck;
        _tail_chunk = ck;
    }

private:
    chunk* _head_chunk{ new chunk };
    chunk* _tail_chunk{ &chunk };

    std::atomic<int> _size{ 0 };
    std::atomic<int> _new_chunk { false };
};