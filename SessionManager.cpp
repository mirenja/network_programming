#include "SessionManager.h"
#include <sstream>
#include <iomanip>

SessionManager::SessionManager() {}

std::string SessionManager::generateSessionId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

std::string SessionManager::createSession(const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string session_id = generateSessionId();
    
    UserSession session;
    session.session_id = session_id;
    session.username = username;
    session.created_at = std::chrono::system_clock::now();
    session.last_active = session.created_at;
    session.message_count = 0;
    
    sessions_[session_id] = session;
    
    return session_id;
}

UserSession* SessionManager::getSession(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessions_.find(session_id);
    if (it != sessions_.end()) {
        return &(it->second);
    }
    return nullptr;
}

void SessionManager::updateActivity(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessions_.find(session_id);
    if (it != sessions_.end()) {
        it->second.last_active = std::chrono::system_clock::now();
    }
}

void SessionManager::cleanupExpiredSessions() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::system_clock::now();
    auto it = sessions_.begin();
    
    while (it != sessions_.end()) {
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(
            now - it->second.last_active
        );
        
        if (duration.count() > 30) {
            it = sessions_.erase(it);
        } else {
            ++it;
        }
    }
}

std::string SessionManager::parseSessionCookie(const std::string& cookie_header) {
    std::string search = "session_id=";
    size_t pos = cookie_header.find(search);
    
    if (pos == std::string::npos) {
        return "";
    }
    
    size_t start = pos + search.length();
    size_t end = cookie_header.find(';', start);
    
    if (end == std::string::npos) {
        return cookie_header.substr(start);
    } else {
        return cookie_header.substr(start, end - start);
    }
}

std::string SessionManager::generateSetCookieHeader(const std::string& session_id) {
    return "session_id=" + session_id + "; Path=/; HttpOnly; Max-Age=1800";
}

void SessionManager::deleteSession(const std::string& session_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(session_id);
}

