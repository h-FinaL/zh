#pragma once

#include <atomic>

template <typename T, unsigned int N = 32>
class freelockqueue
{
    typedef unsigned int index_t;

    struct node
    {
        union
        {
            char value[sizeof(T)];
            node* en_next;
        };
        node* de_next;
    };

    struct chunk
    {
        chunk()
        {
            for (int i = 0; i < N - 1; i++)
            {
                _nodes[i].en_next = &_nodes[i + 1];
                _nodes[i].de_next = nullptr;
            }
            _nodes[N - 1].en_next = nullptr;
            _nodes[N - 1].de_next = nullptr;
        }

        node* tail_node() { return &_nodes[N - 1]; }
        node* head_node() { return &_nodes[0]; }

        node _nodes[N];
        chunk* next{ nullptr };
    };

public:
    freelockqueue()
    {
        chunk* head_chunk = _head_chunk.load();
        chunk* tail_chunk = _tail_chunk.load();
        _en_node.store(&head_chunk->_nodes[0]);
        _de_node.store(&tail_chunk->_nodes[0]);

    }
    ~freelockqueue() {}

    template<typename U>
    void enqueue(U&& value)
    {
        node* en_node;
        do
        {
            en_node = _en_node.load(std::memory_order_relaxed);
            while (en_node->en_next == nullptr)
            {
                bool new_chunk = false;
                if (_new_chunk.compare_exchange_weak(new_chunk, true))
                {
                    en_node = _en_node.load(std::memory_order_relaxed);
                    if (en_node->en_next != nullptr)
                        continue;
                    chunk* tail_chunk = _tail_chunk.load();
                    chunk* ck = new chunk;
                    tail_chunk->next = ck;
                    _tail_chunk.store(ck);

                    en_node->en_next = ck->head_node();
                    _new_chunk.store(true);
                    break;
                }
                en_node = _en_node.load(std::memory_order_relaxed);
            }

        } while (!_en_node.compare_exchange_weak(en_node, en_node->en_next));

        en_node->de_next = en_node->en_next;
        new (en_node->value) T(std::move(value));
    }

    bool try_dequeue(T& value)
    {
        node* de_node;
        do
        {
            de_node = _de_node.load();
            if (de_node->de_next == nullptr)
                return false;
        } while (!_de_node.compare_exchange_weak(de_node, de_node->de_next));

        value = std::move(*(T*)de_node->value);
    }

private:
    std::atomic<chunk*> _head_chunk{ new chunk };
    std::atomic<chunk*> _tail_chunk{ _head_chunk.load() };

    std::atomic<node*> _en_node{ nullptr };
    std::atomic<node*> _de_node{ nullptr };

    std::atomic<bool> _new_chunk{ false };
};