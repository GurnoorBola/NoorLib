/**
 * A Single Reader Single Writer Lock Free Queue Implementation
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <array>
#include <atomic>

template <typename T, std::size_t N>
class Queue {
   public:
    /**
     * Pushes n bits from src starting from offset onto Queue
     * Returns the number of bits pushed
     */
    template <std::size_t I>
    std::size_t push(std::array<T, I>& src, std::size_t n=1, std::size_t offset=0);

    /**
     * Pops n bits from Queue into dest starting from offset
     * Returns the number of bits popped
     */
    template <std::size_t I>
    std::size_t pop(std::array<T, I>& dest, std::size_t n=1, std::size_t offset=0);


   private:
    std::atomic<std::size_t> m_back{N};
    std::atomic<std::size_t> m_front{N};
    std::array<T, N+1> m_buffer{};
};


template <typename T, std::size_t N>
template <std::size_t I>
std::size_t Queue<T, N>::push(std::array<T, I>& src, std::size_t n, std::size_t offset) {
    std::size_t front{m_front.load(std::memory_order_acquire)};
    std::size_t back{m_back.load(std::memory_order_relaxed)};

    std::size_t numPushed{0};
    for (; numPushed < n; ++numPushed) {
        std::size_t next {(back + 1) % (N + 1)};
        if (next == front) break;
        m_buffer[next] = src[offset++];
        back = next;
    }

    m_back.store(back, std::memory_order_release);
    return numPushed;
}

template <typename T, std::size_t N>
template <std::size_t I>
std::size_t Queue<T, N>::pop(std::array<T, I>& dest, std::size_t n, std::size_t offset) {
    std::size_t back{m_back.load(std::memory_order_acquire)};
    std::size_t front{m_front.load(std::memory_order_relaxed)};
    if (front == back) return 0;

    std::size_t numPopped{0};
    for (int i{0}; i < n; ++i) {
        std::size_t next = {(front + 1) % (N + 1)}; 
        dest[offset++] = m_buffer[next];
        front = next;
        ++numPopped;
        if (front == back) break;
    }

    m_front.store(front, std::memory_order_release);
    return numPopped;
}

#endif
