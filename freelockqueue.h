#pragma once

#include <atomic>

template <typename T, unsigned int N = 32>
class freelockqueue
{
    typedef unsigned int index_t;

    enum result_status
    {
        enqueue_complate,
        enqueue_chunk_not_enough,

        dequeue_complate,
        dequeue_chunk_end,
        dequeue_chunk_empty
    };

    struct chunk
    {
        template<typename U>
        result_status enqueue(U&& value)
        {
            index_t tail_index;
            do
            {
                tail_index = _flush_index.load(std::memory_order_relaxed);

                if (tail_index >= N)
                    return enqueue_chunk_not_enough;

            } while (!_flush_index.compare_exchange_weak(tail_index, tail_index + 1, std::memory_order_relaxed));

            new (&_value[tail_index]) T(std::move(value));

            while (!_tail_index.compare_exchange_weak(tail_index, tail_index + 1)) {};
            return enqueue_complate;
        }

        result_status dequeue(T& value)
        {
            index_t head_index;
            index_t tail_index;

            do
            {
                head_index = _head_index.load(std::memory_order_relaxed);
                tail_index = _tail_index.load(std::memory_order_relaxed);

                if (head_index >= N)
                    return dequeue_chunk_end;

                if (head_index >= tail_index)
                    return dequeue_chunk_empty;

            } while (!_head_index.compare_exchange_weak(head_index, head_index + 1, std::memory_order_relaxed));

            value = std::move((T)_value[head_index]);
            return dequeue_complate;
        }

        char _buf[N * sizeof(T)]{ 0 };
        T* _value{ (T*)_buf };
        std::atomic<index_t> _head_index{ 0 };
        std::atomic<index_t> _tail_index{ 0 };
        std::atomic<index_t> _flush_index{ 0 };
        chunk* _next{ nullptr };
    };

public:
    freelockqueue() {}
    ~freelockqueue() {}

    template<typename U>
    void enqueue(U&& vaule)
    {
        chunk* tail_chunk = _tail_chunk.load(std::memory_order_relaxed);
        result_status rs = tail_chunk->enqueue(std::forward<U>(vaule));
        while (rs == enqueue_chunk_not_enough)
        {
            bool new_chunk = false;
            if (_new_chunk.compare_exchange_strong(new_chunk, true))
            {
                tail_chunk = _tail_chunk.load(std::memory_order_relaxed);
                rs = tail_chunk->enqueue(std::move(vaule));
                if (rs == enqueue_chunk_not_enough)
                {
                    /*      std::atomic<chunk_t*> ck;
                          ck.store(new chunk_t);*/
                    chunk* ck = new chunk;
                    _tail_chunk.load(std::memory_order_relaxed)->_next = ck;
                    _tail_chunk.store(ck);
                    _chunk_count++;

                    _new_chunk.store(false, std::memory_order_release);

                    rs = _tail_chunk.load(std::memory_order_relaxed)->enqueue(std::move(vaule));
                    if (rs != enqueue_complate)
                        continue;
                    else
                        break;
                }
                _new_chunk.store(false, std::memory_order_release);
            }
        }
    }

    bool dequeue(T& value)
    {
        chunk* head_chunk;

        do
        {
            head_chunk = _head_chunk.load(std::memory_order_relaxed);
            result_status rs = head_chunk->dequeue(value);

            if (rs == dequeue_complate)
                break;

            if (rs == dequeue_chunk_end)
            {
                if (head_chunk->_next == nullptr)
                    return false;
                _head_chunk.store(head_chunk->_next);
                delete head_chunk;
                _chunk_count--;
                continue;
            }

            if (rs == dequeue_chunk_empty)
                return false;

        } while (1);

        return true;
    }

private:
    std::atomic<chunk*> _head_chunk{ new chunk };
    std::atomic<chunk*> _tail_chunk{ _head_chunk.load(std::memory_order_relaxed) };

    std::atomic<int> _chunk_count{ 0 };
    std::atomic<bool> _new_chunk{ false };
};