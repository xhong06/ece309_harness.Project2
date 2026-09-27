// tests/p2/test_p2.cpp
//
// YOUR test suite goes here. At least 12 assert-based test cases — see
// spec §5 for the required categories and the sample test for the
// expected level of rigor.
//
// This file is a stub so the project builds out of the box; replace the
// body of main() with your own tests.

#include "core/conversation.h"
#include "core/message.h"
#include "core/sentinel_scanner.h"
#include "harness/harness.h"
#include "model/replay_client.h"
#include "model/scripted_client.h"
#include <stdexcept>

#include <cassert>



void handle_empty_conversation() {
    // This test case checks that the Conversation class correctly handles an empty conversation by verifying that size() returns 0 and that accessing an out-of-bounds index throws the expected exception.

    Conversation conv; // Create an empty Conversation object.

    assert(conv.size() == 0 && "Expected size() to be equal to 0 for an empty conversation");
    
    bool exception_thrown = false;  // Set a flag to track if the exception is thrown.
    try {
        conv.at(0); // Attempt to access the first message in an empty conversation, which should throw an exception.
    } catch (const std::out_of_range& e) { // Catch the expected std::out_of_range exception.
        exception_thrown = true; // Set the flag to true to indicate that the exception was thrown as expected.
    }
    assert(exception_thrown && "Expected the exception 'std::out_of_range' when accessing an empty conversation");
        
}

void system_message_ordering() {
    // This test case checks that the Conversation class correctly maintains the order of messages, especially when a System message is appended to the front of the conversation.
    
    Conversation conv; // Create a Conversation object to hold the messages.

    conv.append(Message(Role::System, "System message 1")); // Append a system message to the front of the conversation.
    conv.append(Message(Role::User, "User message 1")); // Append a user message to the conversation.

    conv.append(Message(Role::Assistant, "Assistant message 1")); // Append an assistant message to the conversation.
    conv.append(Message(Role::User, "User message 2")); // Append another user message to the conversation.
    conv.append(Message(Role::Assistant, "Assistant message 2")); // Append another system message to the front of the conversation.
    conv.append(Message(Role::User, "User message 3")); // Append another user message to the conversation.
    conv.append(Message(Role::Assistant, "Assistant message 3")); // Append another assistant message to the conversation.

    assert(conv.at(0).role() == Role::System && "Expected the first message to remain a System message");

}

void rule_of_five_copy() {
    // This test case checks that the Conversation class correctly implements the Rule of Five by verifying that the copy constructor and copy assignment operator create deep copies of Conversation objects, ensuring that changes to one object do not affect the other.

    Conversation conv1; // Create a Conversation object to hold messages.
    conv1.append(Message(Role::System, "System message 1")); // Append a system message to the front of the conversation.
    conv1.append(Message(Role::User, "User message 1")); // Append a user message to the conversation.
    conv1.append(Message(Role::Assistant, "Assistant message 1")); // Append an assistant message to the conversation.

    Conversation conv2(conv1); // Create a new Conversation object using the ***Copy Constructor*** to make a deep copy of conv1.

    assert(conv2.begin() == conv1.begin() && "Expected the first message in conv2 to be equal to the first message in conv1");
    assert(conv2.end() == conv1.end() && "Expected the last message in conv2 to be equal to the last message in conv1");
    assert(&conv2.at(0) != &conv1.at(0) && "Expected the first message in conv2 to be a different object with a different memory address than the first message in conv1 even though they have the same content");
    assert(&conv2.at(1) != &conv1.at(1) && "Expected the second message in conv2 to be a different object with a different memory address than the second message in conv1 even though they have the same content");
    assert(&conv2.at(2) != &conv1.at(2) && "Expected the third message in conv2 to be a different object with a different memory address than the third message in conv1 even though they have the same content");

    conv2.append(Message(Role::User, "User message 4")); // Append a new user message to conv2 to verify that it does not affect conv1.
    assert(conv2.end() != conv1.end() && "Expected the end of conv2 to be different from the end of conv1 after appending a new message to conv2 since conv2 is a deep copy of conv1 and should not affect it");

    Conversation conv3 = conv1; // Create a new Conversation object using the ***Copy Assignment Operator*** to make a deep copy of conv1.
    assert(conv3.begin() == conv1.begin() && "Expected the first message in conv3 to be equal to the first message in conv1");
    assert(conv3.end() == conv1.end() && "Expected the last message in conv3 to be equal to the last message in conv1");
    assert(&conv3.at(0) != &conv1.at(0) && "Expected the first message in conv3 to be a different object with a different memory address than the first message in conv1 even though they have the same content");
    assert(&conv3.at(1) != &conv1.at(1) && "Expected the second message in conv3 to be a different object with a different memory address than the second message in conv1 even though they have the same content");
    assert(&conv3.at(2) != &conv1.at(2) && "Expected the third message in conv3 to be a different object with a different memory address than the third message in conv1 even though they have the same content");
    conv3.append(Message(Role::Assistant, "Assistant message 4")); // Append a new assistant message to conv3 to verify that it does not affect conv1.
    assert(conv3.end() != conv1.end() && "Expected the end of conv3 to be different from the end of conv1 after appending a new message to conv3 since conv3 is a deep copy of conv1 and should not affect it");

}

void rule_of_five_move() {
    // This test case checks that the Conversation class correctly implements the Rule of Five by verifying that the move constructor and move assignment operator transfer ownership of resources from one Conversation object to another, leaving the moved-from object in a valid but empty state.
    
    Conversation conv1; // Create a Conversation object to hold messages.
    conv1.append(Message(Role::System, "System message 1")); // Append a system message to the front of the conversation.
    conv1.append(Message(Role::User, "User message 1")); // Append a user message to the conversation.
    conv1.append(Message(Role::Assistant, "Assistant message 1")); // Append an assistant message to the conversation.

    std::size_t original_size = conv1.size(); // Store the original size of conv1 to verify that it is transferred to conv2 after the move.
    const Message* original_begin = conv1.begin(); // Store the original begin pointer of conv1 to verify that it is transferred to conv2 after the move.

    Conversation conv2(std::move(conv1)); // Create a new Conversation object using the ***Move Constructor*** to transfer ownership of conv1's resources to conv2.
    assert(conv2.size() == original_size && "Expected the size of conv2 to be equal to the original size of conv1 from before the move");
    assert(conv2.begin() == original_begin && "Expected the first message in conv2 to be equal to the original first message in conv1 from before the move");
    assert(conv2.begin() != conv1.begin() && "Expected the first message in conv2 to be a different object with a different memory address than the first message in conv1 since conv1 does not own any messages after the move");
    assert(conv2.end() != conv1.end() && "Expected the last message in conv2 to be a different object with a different memory address than the last message in conv1 since conv1 does not own any messages after the move");
    assert(conv1.size() == 0 && "Expected the size of conv1 to be 0 since all its messages have been transferred to conv2 after the move");
    assert(conv1.begin() == nullptr && "Expected the first message in conv1 to be nullptr since it should not own any messages after the move");
    assert(conv1.begin() == conv1.end() && "Expected the first message in conv1 to be equal to the last message in conv1 since it should not own any messages after the move");

    std::size_t original_size2 = conv2.size(); // Store the original size of conv2 to verify that it is not changed after the move.
    const Message* original_begin2 = conv2.begin(); // Store the original begin pointer of conv2 to verify that it is not changed after the move.
    Conversation conv3; // Create a new Conversation object to hold messages.
    conv3 = std::move(conv2); // Use the ***Move Assignment Operator*** to transfer ownership of conv2's resources to conv3.
    assert(conv3.size() == original_size2 && "Expected the size of conv3 to be equal to the original size of conv2 from before the move");
    assert(conv3.begin() == original_begin2 && "Expected the first message in conv3 to be equal to the original first message in conv2 from before the move");
    assert(conv3.begin() != conv2.begin() && "Expected the first message in conv3 to be a different object with a different memory address than the first message in conv2 since conv2 does not own any messages after the move");
    assert(conv3.end() != conv2.end() && "Expected the last message in conv3 to be a different object with a different memory address than the last message in conv2 since conv2 does not own any messages after the move");
    assert(conv2.size() == 0 && "Expected the size of conv2 to be 0 since all its messages have been transferred to conv3 after the move");
    assert(conv2.begin() == nullptr && "Expected the first message in conv2 to be nullptr since it should not own any messages after the move");
    assert(conv2.begin() == conv2.end() && "Expected the first message in conv2 to be equal to the last message in conv2 since it should not own any messages after the move");
}

void growth_behavior() {
    // This test case checks that the Conversation class correctly implements the growth behavior of its backing array by verifying that the capacity of the array increases when the size exceeds the current capacity, and that the capacity doubles when it grows.
    
    Conversation conv; // Create a Conversation object to hold messages.
    std::size_t initial_capacity = conv.capacity();; // Store the initial capacity of the conversation
    for (std::size_t i = 0; i < 100; i++) { // Append 100 messages to the conversation in order to test the growth behavior of the backing array.
        conv.append(Message(Role::User, "User message placeholder")); // Append a user message to the conversation.
        if (conv.size() > initial_capacity) { // Check if the size of the conversation has exceeded the initial capacity.
            assert(conv.capacity() > initial_capacity && "Expected the capacity of the conversation to increase when the size exceeds the initial capacity"); // Test 1: Verify that the capacity of the conversation has increased when the size exceeds the initial capacity.
            assert(conv.capacity() == initial_capacity * 2 && "Expected the capacity of the conversation to double when the size exceeds the initial capacity"); // Test 2: Verify that the capacity of the conversation has doubled when the size exceeds the initial capacity.
            initial_capacity = conv.capacity(); // Update the initial capacity to the new capacity after growth.
        }
    }

}

void scanner_clean_text() {
    // This test case checks that the Sentinel Scanner class correctly identifies and returns the safe text from a given input string that does not contain the sentinel, ensuring that the entire input is returned as safe text when the sentinel is not present.
    
    const std::string sentinel = "<|end_conversation|>";
    const std::string text = "Hello, world! Isn't the weather just lovely today? I can't believe it's fall already. Though it is about time since the humidity was absolutely unbearable this summer. I can't wait to go apple picking and enjoy some pumpkin spice lattes. What are your plans for the fall season? I hope you have a wonderful time and enjoy all the beautiful colors of the leaves. Take care and stay warm!";
    SentinelScanner scanner(sentinel);
    auto result = scanner.feed(text);
    assert(!result.sentinel_found && "Expected the sentinel to not be found in the text");
    assert(result.safe_text == text && "Expected the safe text to match the input text");
}



void scanner_catches_sentinel_at_every_boundary() {
    // This test case checks that the Sentinel Scanner class can correctly indentify the sentinel even if it is split across chunks.
    
    const std::string sentinel = "<|end_conversation|>";
    const std::string text = "Goodbye." + sentinel;
    for (std::size_t split = 0; split <= text.size(); ++split) {
        SentinelScanner scanner(sentinel);
        auto out1 = scanner.feed(text.substr(0, split));
        auto out2 = scanner.feed(text.substr(split));
        assert((out1.sentinel_found || out2.sentinel_found) &&
            "sentinel must be caught regardless of split point");
        assert(out1.safe_text + out2.safe_text == "Goodbye.");
    }
}

void scanner_false_alarm() {
    // This test case checks that the Sentinel Scanner class can recognize when an input is close to the sentinel but not exactly the sentinel. Should return false for sentinel_found and output the full text safely.

    const std::string sentinel = "<|end_conversation|>";
    const std::string text = "Goodbye. <|end_world|> Haha just kidding.";
    SentinelScanner scanner(sentinel);
    auto out1 = scanner.feed(text);
    assert(!out1.sentinel_found && "The sentinel should not be found in the text");
    assert(out1.safe_text == text && "The safe text should match the input text");
}

void scanner_bounded_memory() {
    const std::string sentinel = "<|end_conversation|>";
    const std::string text = "<|end_conversa";
    SentinelScanner scanner(sentinel);
    auto out1 = scanner.feed(text);
    assert()
}


int main() {
    
    

    
}
