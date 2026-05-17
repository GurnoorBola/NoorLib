#ifndef QUEUE_H
#define QUEUE_H

#include <algorithm>
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
    std::size_t push(std::size_t n, std::array<T, I>& src, std::size_t offset);

    /**
     * Pushes all elements in src from offset onward onto Queue
     * Returns the number of bits pushed
     */
    template <std::size_t I>
    std::size_t push(std::array<T, I>& src, std::size_t offset);

    /**
     * Pops n bits from Queue into dest starting from offset
     * Returns the number of bits popped
     */
    template <std::size_t I>
    std::size_t pop(std::size_t n, std::array<T, I>& dest, std::size_t offset);

    /**
     * Pops from Queue into dest starting from offset until Queue is empty or dest is full
     * Returns the number of bits popped
     */
    template <std::size_t I>
    std::size_t pop(std::array<T, I>& dest, std::size_t offset);

   private:
    std::atomic<std::size_t> m_back{N};
    std::atomic<std::size_t> m_front{N};
    std::array<T, N+1> m_buffer{};
};

#endif

#ifdef QUEUE_IMPL

template <typename T, std::size_t N>
template <std::size_t I>
std::size_t Queue<T, N>::push(std::size_t n, std::array<T, I>& src, std::size_t offset) {
    std::size_t front{m_front.load()};

    std::size_t numPushed{0};
    for (; numPushed < n; ++numPushed) {
        std::size_t next {(m_back + 1) % (N + 1)};
        if (next == front) break;
        m_back = next;
        m_buffer[m_back] = src[offset++];
    }

    return numPushed;
}

template <typename T, std::size_t N>
template <std::size_t I>
std::size_t Queue<T, N>::push(std::array<T, I>& src, std::size_t offset) {
    return push(src.size() - offset, src, offset);
}

template <typename T, std::size_t N>
template <std::size_t I>
std::size_t Queue<T, N>::pop(std::size_t n, std::array<T, I>& dest, std::size_t offset) {
    std::size_t back{m_back.load()};
    if (m_front == back) return 0;

    std::size_t numPopped{0};
    for (; numPopped < n; ++numPopped) {
        m_front = (m_front + 1) % (N + 1); 
        dest[offset++] = m_buffer[m_front];
        if (m_front == back) return numPopped + 1;
    }

    return numPopped;
}

template <typename T, std::size_t N>
template <std::size_t I>
std::size_t Queue<T, N>::pop(std::array<T, I>& dest, std::size_t offset) {
    return pop(dest.size() - offset, dest, offset);
}

#endif
