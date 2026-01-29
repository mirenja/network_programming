#pragma once

#include <string>
#include <map>
#include <mutex>
#include <chrono>
#include <random>

struct UserSession {
    std::string session_id;
    std::string username;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point last_active;
    int message_count;
};

class SessionManager {
public:
    SessionManager();
    
    std::string createSession(const std::string& username);

    UserSession* getSession(const std::string& session_id);
    void updateActivity(const std::string& session_id);
    
    void cleanupExpiredSessions();
    void deleteSession(const std::string& session_id);
    std::string parseSessionCookie(const std::string& cookie_header);
    std::string generateSetCookieHeader(const std::string& session_id);
    
private:
    std::map<std::string, UserSession> sessions_;
    mutable std::mutex mutex_;
    
    std::string generateSessionId();
};