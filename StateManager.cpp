#include "StateManager.h"
#include <fstream>
#include <iostream>
#include <sstream>

StateManager::StateManager(const std::string& filename) 
    : filename_(filename), auto_save_enabled_(false) {}

std::string StateManager::serializeMessage(const ChatMessage& msg) {
    // Format: username|timestamp|message
    std::string username = msg.username;
    std::string message = msg.message;
    std::string timestamp = msg.timestamp;
    
    return username + "|" + timestamp + "|" + message;
}

ChatMessage StateManager::deserializeMessage(const std::string& line) {
    ChatMessage msg;
    std::vector<std::string> parts;
    std::string current;
    
    
    // Split by unescaped | also push_back same as append
    for (size_t i = 0; i < line.length(); ++i) {
        if (line[i] == '|'){
            parts.push_back(current);
            current.clear();
        } else {
            current += line[i];
        }
    }
    
    parts.push_back(current);
    
    if (parts.size() >= 2) {
        msg.username = parts[0];
        msg.timestamp = parts[1];
        msg.message = parts[2];
        // incase there is an escaped one we need to handle that also
        //for (size_t i = 3; i < parts.size(); ++i) {
        //     msg.message += "|" + parts[i];
        // }
    }
    
    return msg;
}

bool StateManager::saveMessages(const std::vector<ChatMessage>& messages) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ofstream file(filename_, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename_ << std::endl;
        return false;
    }
    
    for (const auto& msg : messages) {
        file << serializeMessage(msg) << "\n";
    }
    
    file.close();
    std::cout << "Saved " << messages.size() << " messages to " << filename_ << std::endl;
    return true;
}

std::vector<ChatMessage> StateManager::loadMessages() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<ChatMessage> messages;
    std::ifstream file(filename_);
    
    if (!file.is_open()) {
        std::cout << "No existing state file found, starting fresh" << std::endl;
        return messages;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            ChatMessage msg = deserializeMessage(line);
            if (!msg.username.empty()) {
                messages.push_back(msg);
            }
        }
    }
    
    file.close();
    std::cout << "Loaded " << messages.size() << " messages from " << filename_ << std::endl;
    return messages;
}

void StateManager::enableAutoSave(int interval_seconds) {
    auto_save_enabled_ = true;
   
    // spawn a thread that periodically saves, like a background thread
}

void StateManager::disableAutoSave() {
    auto_save_enabled_ = false;
}