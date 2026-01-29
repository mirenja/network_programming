#pragma once

#include <string>
#include <vector>
#include <mutex>
#include "ChatHandler.h"

class StateManager {
public:
    StateManager(const std::string& filename = "chat_state.dat");
    
    bool saveMessages(const std::vector<ChatMessage>& messages);
    
    std::vector<ChatMessage> loadMessages();
    
    void enableAutoSave(int interval_seconds = 60);
    void disableAutoSave();
    
private:
    std::string filename_;
    std::mutex mutex_;
    bool auto_save_enabled_;
    
    std::string serializeMessage(const ChatMessage& msg);
    
    ChatMessage deserializeMessage(const std::string& line);
};