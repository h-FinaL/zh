#pragma once

#include <atomic>
#include <mutex>

template <typename T, unsigned int N = 32>
class freelockqueue
{
    typedef unsigned int index_t;

    struct node_t
    {
        char value[sizeof(T)];
        std::atomic<node_t*> next{ nullptr };
        std::atomic<bool> have_value{ false };
    };

    struct chunk
    {
        chunk()
        {
            for (int i = 0; i < N - 1; i++)
            {
                _nodes[i].next = &_nodes[i + 1];
            }
        }


        node_t _nodes[N];
        chunk* next{ nullptr };
    };

public:
    freelockqueue() {}
    ~freelockqueue() {}

    template<typename U>
    void enqueue(U&& value)
    {
        node_t* node;
        node_t* next_node;
        while (1)
        {
            node = _tail_node.load(std::memory_order_relaxed);
            next_node = node->next.load(std::memory_order_relaxed);

            if (next_node == nullptr)
            {
                if (!_new_chunk_lock.try_lock())
                    continue;

                node = _tail_node.load(std::memory_order_relaxed);
                next_node = node->next.load(std::memory_order_relaxed);

                if (next_node != nullptr)
                {
                    _new_chunk_lock.unlock();
                    continue;
                }

                chunk* new_chunk = new chunk;
                _tail_chunk.load()->next = new_chunk;
                _tail_chunk.store(new_chunk);

                next_node = new_chunk->_nodes;
                node->next.store(next_node, std::memory_order_seq_cst);

                _new_chunk_lock.unlock();
            }

            if (_tail_node.compare_exchange_weak(node, next_node))
                break;
        };

        new (&node->value) T(std::move(value));
        node->have_value.store(true, std::memory_order_release);
    }

    bool try_dequeue(T& value)
    {
        node_t* node;
        node_t* next_node;
        bool have_value;
        do
        {
            node = _head_node.load();
            have_value = node->have_value.load();
            next_node = node->next.load();
            if (have_value == false)
                return false;
        } while (!_head_node.compare_exchange_weak(node, next_node));
        

        value = std::move(*(T*)node->value);
        return true;
    }

private:
    std::atomic<chunk*> _head_chunk{ new chunk };
    std::atomic<chunk*> _tail_chunk{ _head_chunk.load() };

    std::atomic<node_t*> _head_node{ _head_chunk.load()->_nodes };
    std::atomic<node_t*> _tail_node{ _head_chunk.load()->_nodes };

    std::mutex _new_chunk_lock;
};