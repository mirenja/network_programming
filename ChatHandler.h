#pragma once

#include <string>
#include <vector>
#include <mutex>

struct ChatMessage {
    std::string username;
    std::string message;
    std::string timestamp;
};

class ChatHandler {
public:
    ChatHandler();
    void addMessage(const std::string& username, const std::string& message);
    std::vector<ChatMessage> getMessages() const;
    std::string generateChatPage() const;
    
private:
    mutable std::mutex mutex_;
    std::vector<ChatMessage> messages_;
    
    std::string getCurrentTimestamp() const;
};