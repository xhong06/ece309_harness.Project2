#include "core/sentinel_scanner.h"
#include "core/message.h"
#include <string>
#include <iostream>

class SentinelScanner {
    
    private:
    std::string sentinel_; 
    std::string pending_;

    public:
    explicit SentinelScanner(std::string sentinel) : sentinel_(sentinel), pending_("") {}


    struct Out { std::string safe_text; bool sentinel_found;}; // Create the Out struct to hold the safe_text which is found through the feed() function and a boolean variable to indicate whether the sentinel has been found or not.

    Out feed(std::string_view chunk) {
        std::string combined = pending_ + std::string(chunk); // Combine the pending string with the new chunk to check for the sentinel. Initially, the pending string will be empty, with the only text in combined being the first chunk; however, as we process chunks, it may be that blocks of chunks get held back so we don't accidentally include part of the sentinel in our ouput. 
        std::size_t sentinel_pos = combined.find(sentinel_); // Find the position of the sentinel in the combined string.
        if(sentinel_pos != std::string::npos) { // Since string.find() returns string::npos if the substring is not found, we check if the sentinel was found in the combined string.
            std::string safe_text = combined.substr(0, sentinel_pos); // Store all the text before the sentinel into the safe_text variable to be returned.
            pending_ = ""; // Clear the pending string since the sentinel has been found and we don't need to hold back any more characters.
            return {safe_text, true}; // Return the safe_text and indicate that the sentinel has been found.
        } else {
            std::size_t max_pending_length = sentinel_.size() - 1; // Calculate the maximum length of the pending string to hold back.
            if(combined.size() > max_pending_length) { // If the combined string is longer than the maximum pending length, we can safely return the text before the last max_pending_length characters.
                std::string safe_text = combined.substr(0, combined.size() - max_pending_length); // Store all the text before the last max_pending_length character into the safe_text variable to be returned.
                pending_ = combined.substr(combined.size() - max_pending_length); // Store all the text after the last max_pending_length character into the pending string to be held back for the next feed() function call.
                return {safe_text, false}; // Return the safe_text and indicate that the sentinel has not been found yet.
            } else {
                pending_ = combined; // If the combined string is shorter than or equal to the maximum pending length, we hold back the entire combined string in the pending variable for the next feed() function call. 
                //This is important because it ensures that we don't accidentally include part of the sentinel in our output.
                return {"", false}; // Return an empty string and indicate that the sentinel has not been found yet.
            }
        }
    }

    Out flush() {
        std::string safe_text = pending_; // Store the remaining text in the pending variable into the safe_text variable to be returned. By this point, we know that the sentinel has not been found, so we can safely return all the text that has been held back in the pending variable.
        pending_ = ""; // Clear the pending variable since we just transferred its contents to the safe_text variable to be returned soon.
        return {safe_text, false}; // Return the safe_text and indicate that the sentinel has not been found yet.
        
    }



    


};