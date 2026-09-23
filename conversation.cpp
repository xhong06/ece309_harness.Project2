

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
        
    Conversation(std::size_t capacity) {
        if(capacity > 0) {
            data_ = new Message[capacity];
            size_ = 0;
            capacity_ = capacity;
        } else {
            data_ = nullptr;
            size_ = 0;
            capacity_ = 0;
        }
    }

    ~Conversation() {
        delete[] data_;
    }

    Conversation(const Conversation& other) 
        : size_(other.size_), capacity_(other.capacity_) {
        if (capacity_ > 0) {
            data_ = new Message[capacity_];
            for (std::size_t i = 0; i < size_; i++) {
                data_[i] = other.data_[i];
            }
        } else {
            data_ = nullptr;
        }
    }

    Conversation & operator=(const Conversation& other) {
        if (this != &other) {
            if(other.capacity_ > 0){
                delete[] data_;
                size_ = other.size_;
                capacity_ = other.capacity_;
                data_ = new Message[capacity_];
                for (std::size_t i = 0; i < size_; i++) {
                    data_[i] = other.data_[i];
                }
            } else {
                delete[] data_;
                data_ = nullptr;
                size_ = 0;
                capacity_ = 0;
            }
            
        }
        return *this;
    }

    Conversation(Conversation&& other) noexcept {
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    Conversation& operator=(Conversation&& other) noexcept {
        if(this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }
};
