
#include "conversation.h"
#include "core/message.h"
#include <string>
#include <iostream>

class Conversation {

    private:
    Message*    data_;
    std::size_t size_;
    std::size_t capacity_;

    public:
    Conversation(): data_(nullptr), size_(0), capacity_(0) {} 
        
    Conversation(Message* data, std::size_t size, std::size_t capacity) {
        data_ = data;
        size_ = size;
        capacity_ = capacity;
    }

    ~Conversation() {
        delete[] data_;
    }
};