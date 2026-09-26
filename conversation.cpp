
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
    Conversation(): data_(nullptr), size_(0), capacity_(0) {}  // Default constructor initializes an empty Conversation with no allocated memory.
        
    Conversation(std::size_t capacity) { 
        if(capacity > 0) { // If the specified capacity is greater than 0, allocate memory for the data_ array.
            data_ = new Message[capacity]; // Allocate a new Message array with the specified capacity.
            size_ = 0; // Initialize the size_ variable to 0 since there are no messages yet.
            capacity_ = capacity; // Set the capacity_ variable to the specified capacity.
        } else {
            data_ = nullptr; // If the specified capacity is 0, set data_ to nullptr to indicate no allocated memory.
            size_ = 0; // Initialize the size_ variable to 0 since there are no messages.
            capacity_ = 0; // Initialize the capacity_ variable to 0 since there are no allocated messages.
        }
    }

    ~Conversation() {
        delete[] data_; // Free the allocated memory for the data_ array to avoid memory leaks.
    }

    Conversation(const Conversation& other) 
        : size_(other.size_), capacity_(other.capacity_) { // Initialize size_ and capacity_ with the values from the other Conversation.
        if (capacity_ > 0) { // If the other Conversation has a non-zero capacity, allocate a new array and copy the messages.
            data_ = new Message[capacity_]; // Allocate a new Message array with the same capacity as the other Conversation.
            for (std::size_t i = 0; i < size_; i++) {
                data_[i] = other.data_[i]; // Copy the existing messages from the other Conversation to the new data_ array.
            }
        } else {
            data_ = nullptr; // If the other Conversation has a capacity of 0, set data_ to nullptr.
        }
    }

    Conversation & operator=(const Conversation& other) {
        if (this != &other) { // Check for self-assignment
            if(other.capacity_ > 0){ // If the other Conversation has a non-zero capacity, allocate a new array and copy the messages.
                delete[] data_; // Free the existing data_ array to avoid memory leaks.
                size_ = other.size_; // Set the size_ variable to the other Conversation's size_.
                capacity_ = other.capacity_; // Set the capacity_ variable to the other Conversation's capacity_.
                data_ = new Message[capacity_]; // Allocate a new Message array with the same capacity as the other Conversation.
                for (std::size_t i = 0; i < size_; i++) { 
                    data_[i] = other.data_[i]; // Copy the existing messages from the other Conversation to the new array.
                }
            } else {
                delete[] data_; // Free the existing data_ array to avoid memory leaks.
                data_ = nullptr; // Set the data_ pointer to nullptr since the other Conversation has a capacity of 0.
                size_ = 0; // Set the size_ variable to 0 since the other Conversation has no messages.
                capacity_ = 0; // Set the capacity_ variable to 0 since the other Conversation has no messages.
            }
            
        }
        return *this;
    }

    Conversation(Conversation&& other) noexcept {
        data_ = other.data_; // Set the data_ pointer to the other Conversation's data_ array.
        size_ = other.size_; // Set the size_ variable to the other Conversation's size_.
        capacity_ = other.capacity_; // Set the capacity_ variable to the other Conversation's capacity_.
        other.data_ = nullptr; // Nullify the other Conversation's data_ pointer (because ownership has been transferred) to avoid double deletion.
        other.size_ = 0; //Nullify the other Conversation's size_ variable (because it no longer owns any messages).
        other.capacity_ = 0; // Nullify the other Conversation's capacity_ variable (because it no longer owns any messages).
    }

    Conversation& operator=(Conversation&& other) noexcept {
        if(this != &other) { // Check for self-assignment
            delete[] data_; // Free the existing data_ array to avoid memory leaks.
            data_ = other.data_; // Set the data_ pointer to the other Conversation's data_ array.
            size_ = other.size_; // Set the size_ variable to the other Conversation's size_.
            capacity_ = other.capacity_; // Set the capacity_ variable to the other Conversation's capacity_.
            other.data_ = nullptr; // Nullify the other Conversation's data_ pointer (because ownership has been transferred) to avoid double deletion.
            other.size_ = 0; // Reset the other Conversation's size_ to 0 (because it no longer owns any messages).
            other.capacity_ = 0; // Reset the other Conversation's capacity_ to 0 (because it no longer owns any messages).
        }
        return *this;
    }

    void append(Message m) {
        if(size_ == capacity_) {
            std:: size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2; // Double the capacity when full. If the capacity is 0, set it to 1. Else, double the capacity.
            Message * new_data = new Message[new_capacity]; // Allocate a new Message array with the new capacity.
            for(std::size_t i = 0; i < size_; i++) {
                new_data[i] = data_[i]; // Copy the existing messages to the new array.
            }
            delete[] data_; // Free the old array.
            data_ = new_data; // Set the data_ pointer to the new array full of the existing messages.
            capacity_ = new_capacity; // Update the capacity variable to the new capacity.
        }
        data_[size_] = m; // Add the new message to the end of the array.
        size_++; // Increment the size variable to reflect the new message.
    }

    std::size_t size() const noexcept {
        return size_; // Return the current number of messages in the conversation.
    }

    const Message& at(std::size_t i) const {
        if(i >= size_) { // Check if the index specified is out of bounds (greater than or equal to the current size of the conversation).
            throw std::out_of_range("Index out of range"); // Throw an exception if the index is out of bounds.
        }
        return data_[i]; // Return a reference to the message at the specified index. 
        //Returning a reference is helpful here because it avoids copying the message, 
        //which avoids the need to allocate memory for a new message and copy the contents of the existing message into it. 
        //This can be more efficient, especially for large messages or when accessing messages frequently.
    }

    const Message* begin() const noexcept {
        return data_; // Return a pointer to the first message in the conversation. This allows for range-based for loops and other iterator-based operations.
    }

    const Message* end() const noexcept {
        return data_ + size_; // Return a pointer to the position just past the last message in the conversation. This allows for range-based for loops and other iterator-based operations.
    }
};
