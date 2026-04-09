#pragma once
#include <cstdint>

#include <string>
#include <vector>

template <typename T> class Stack {
  public:
    std::vector<T> s;
    size_t size = 0;
    void push(T hash) {
        s.push_back(hash);
        size++;
    }
    void pop() {
        size--;
    }
    int containsCount(T v) {
        int count = 0;
        for (size_t i = 0; i < size; i++) {
            if (s[i] == v) {
                count++;
            }
        }
        return count;
    }
    void clear() {
        s.clear();
        size = 0;
    }
    bool empty() {
        return size == 0;
    }
    T get(size_t index) {
        return s[index];
    }
    T top() {
        return get(size - 1);
    }
};

/**
 * @brief Raise a critical error and exit
 *
 * @param message The message to display to stdout
 * @param exit_code Exit code, usually 1 or 2
 */
void fce_error(const std::string &message, int exit_code);