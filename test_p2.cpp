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


#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace {

    // Lightweight InputSource feeding lines from a vector
    class VectorInputSource : public InputSource {
    public:
        explicit VectorInputSource(std::vector<std::string> lines)
            : lines_(std::move(lines)) {}

        std::string read_line() override {
            if (index_ >= lines_.size()) {
                eof_ = true;
                return "";
            }
            return lines_[index_++];
        }

        bool is_eof() const override { return eof_; }

    private:
        std::vector<std::string> lines_;
        std::size_t index_ = 0;
        bool eof_ = false;
    };

    // Discards terminal output during test runs
    class QuietOutputSink : public OutputSink {
    public:
        void write(std::string_view /*text*/) override {}
    };

} // namespace

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
    const std::size_t max_allowed_pending = sentinel.size() - 1; // 19 characters
    const std::string text = "<|end_conversa";
    SentinelScanner scanner(sentinel);

    std::size_t input_length = 0; // Create a variable to track the size of the text to be processed by scanner.
    std::size_t output_length = 0; // Create a variable to track the size of the text outputted by the scanner after being processed for the sentinel.

    for(int i = 0; i < 10000; i++) { // For a long long time...
        auto out1 = scanner.feed(text); // Feed the close-to-sentinel text into the scanner to check for sentinel.
        input_length += text.size(); // Increment the total input size by adding the size of the chunk being processed in this cycle of the loop. +14 each loop.
        output_length += out1.safe_text.size(); // Increment the total output size by adding the size of the bit of the chunk being moved into the safe_text during this cycle of the loop.
        assert(!out1.sentinel_found && "Expected not to find sentinel in the close-to-sentinel text."); // Check for sentinel.

        std::size_t current_held_bytes = input_length - output_length; // Create a variable to track the size of the chunk in the pending_ instance variable within the SentinelScanner.
        assert(current_held_bytes <= max_allowed_pending && "Error: Sentinel Scanner held back more bytes in pending_ than sentinel.size() - 1 after breaking apart the chunk.");
        // Check if the current bytes held in pending ever exceed what is permitted.

    }
}

void harness_turn_limit() {
    HarnessConfig exampleConfiguration; // Create an example harness configuration for testing purposes.
    exampleConfiguration.max_turns = 3; // Since the harness is for testing purposes, we're going to make the max turns very small so that we don't need an enormous amount of messages to test turn-limit.
    exampleConfiguration.system_message = "You may begin anytime";

    VectorInputSource input({"User input 1", "User input 2", "User input 3", "User input 4"});
    QuietOutputSink output;

    auto model = std::make_unique<ScriptedModelClient>("sample_input10.txt");

    Harness harness(std::move(model), exampleConfiguration);

    StopReason result = harness.run(input, output);
    assert(result.kind == StopReason::Kind::TurnLimit && "Expected to be stopped because max_turns is reached");
    // Check to see if Stop-reason is because of turn-limit.

    const Conversation& conv = harness.conversation();
    assert(conv.size() == 5 && "Conversation size must equal 5 messages");

    assert(conv.at(0).role() == Role::System);
    assert(conv.at(1).role() == Role::User && conv.at(1).content() == "User input 1");
    assert(conv.at(2).role() == Role::Assistant && conv.at(2).content() == "First assistant reply");
    assert(conv.at(3).role() == Role::User && conv.at(3).content() == "User input 2");
    assert(conv.at(4).role() == Role::Assistant && conv.at(4).content() == "Second assistant reply");
    
}

void harness_sentinel_halt() {
    HarnessConfig exampleConfiguration;
    exampleConfiguration.max_turns = 10;

    VectorInputSource input({"User input 1", "User input 2", "User input 3"});
    QuietOutputSink output;

    auto model = std::make_unique<ScriptedModelClient>("sample_input11.txt");
    Harness harness(std::move(model), exampleConfiguration);

    StopReason result = harness.run(input, output);
    assert(result.kind == StopReason::Kind::Sentinel && "Expected to be stopped because sentinel is identified");
    // Check to see if Stop-reason is because of Sentinel.

    const Conversation& conv = harness.conversation();
    assert(conv.size() == 4 && "Conversation must contain 2 User and 2 Assistant messages");

    assert(conv.at(3).role() == Role::Assistant);
    assert(conv.at(3).content() == "This is turn two.<|end_conversation|>" &&
           "Stored assistant message must include the sentinel and strip anything trailing it");
}

void transcript_round_trip() {
    auto model = std::make_unique<ReplayModelClient>("sample_input12.txt");
    HarnessConfig exampleConfiguration;
    exampleConfiguration.max_turns = 5;
    exampleConfiguration.system_message = "You may begin anytime";

    VectorInputSource input({"Prompt 1", "Prompt 2", "Prompt 3"});
    QuietOutputSink output;

    Harness harness(std::move(model), exampleConfiguration);
    StopReason result = harness.run(input, output);
    assert(result.kind == StopReason::Kind::Sentinel && "Expected to be stopped because sentinel is identified while replaying transcript");

    const Conversation& conv = harness.conversation();
    assert(conv.size() == 5 && "Replayed conversation must contain 5 total messages");

    assert(conv.at(0).role() == Role::System && 
           conv.at(0).content() == "You are a deterministic replay bot.");
    
    assert(conv.at(1).role() == Role::User && conv.at(1).content() == "Prompt 1");
    assert(conv.at(2).role() == Role::Assistant && conv.at(2).content() == "Replayed reply 1");
    
    assert(conv.at(3).role() == Role::User && conv.at(3).content() == "Prompt 2");
    assert(conv.at(4).role() == Role::Assistant && conv.at(4).content() == "Replayed reply 2<|end_conversation|>");
}


int main() {
    handle_empty_conversation();
    system_message_ordering();
    rule_of_five_copy();
    rule_of_five_move();
    growth_behavior();
    scanner_clean_text();
    scanner_catches_sentinel_at_every_boundary();
    scanner_false_alarm();
    scanner_bounded_memory();
    harness_turn_limit();
    harness_sentinel_halt();
    transcript_round_trip();

}
