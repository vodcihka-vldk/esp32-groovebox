#pragma once

#include <cstddef>
#include <array>

template<typename T, size_t SIZE>
class RingBuffer {
public:
    RingBuffer() : m_head(0), m_tail(0), m_count(0) {}
    
    bool write(const T& item) {
        if (isFull()) return false;
        
        m_buffer[m_head] = item;
        m_head = (m_head + 1) % SIZE;
        m_count++;
        return true;
    }
    
    bool read(T& item) {
        if (isEmpty()) return false;
        
        item = m_buffer[m_tail];
        m_tail = (m_tail + 1) % SIZE;
        m_count--;
        return true;
    }
    
    bool isEmpty() const { return m_count == 0; }
    bool isFull() const { return m_count == SIZE; }
    size_t available() const { return SIZE - m_count; }
    size_t count() const { return m_count; }
    
    // Peek without removing
    const T& peek() const {
        return m_buffer[m_tail];
    }
    
    // Clear buffer
    void clear() {
        m_head = 0;
        m_tail = 0;
        m_count = 0;
    }
    
private:
    std::array<T, SIZE> m_buffer;
    size_t m_head;
    size_t m_tail;
    size_t m_count;
};
