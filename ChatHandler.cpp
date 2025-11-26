#include "ChatHandler.h"
#include <sstream>
#include <chrono>
#include <iomanip>
#include <ctime> 

ChatHandler::ChatHandler() {
    addMessage("System", "Welcome to the chat room!");
}

void ChatHandler::addMessage(const std::string& username, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ChatMessage msg;
    msg.username = username;
    msg.message = message;
    msg.timestamp = getCurrentTimestamp();
    
    messages_.push_back(msg);
}

std::vector<ChatMessage> ChatHandler::getMessages() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return messages_;
}

std::string ChatHandler::generateChatPage() const {
    std::ostringstream html;
    
    html << R"(
<!DOCTYPE html>
<html>
<head>
    <title>Chat Room yaay!</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            background-color: #f5f5f5;
        }
        h1 {
            color: #333;
        }
        .chat-container {
            background: white;
            border-radius: 8px;
            padding: 20px;
            margin-bottom: 20px;
            max-height: 400px;
            overflow-y: auto;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        .message {
            margin: 10px 0;
            padding: 10px;
            background: #e8f4f8;
            border-radius: 4px;
        }
        .username {
            font-weight: bold;
            color: #0066cc;
        }
        .timestamp {
            font-size: 0.8em;
            color: #666;
        }
        .input-form {
            background: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        input, textarea {
            width: 100%;
            padding: 10px;
            margin: 5px 0;
            box-sizing: border-box;
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        button {
            background-color: #0066cc;
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 16px;
        }
        button:hover {
            background-color: #0052a3;
        }
        a {
            color: #0066cc;
            text-decoration: none;
        }
    </style>
</head>
<body>
    <h1>Chat Room</h1>
    
    <div class="chat-container">
)";
    
    
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& msg : messages_) {
        html << "        <div class=\"message\">\n"
             << "            <span class=\"username\">" << msg.username<< "</span>\n"
             << "            <span class=\"timestamp\">(" << msg.timestamp << ")</span>\n"
             << "            <div>" << msg.message << "</div>\n"
             << "        </div>\n";
    }
    
    html << R"(    </div>
    
    <div class="input-form">
        <form method="POST" action="/chat/send">
            <label for="username">Username:</label>
            <input type="text" id="username" name="username" required>
            
            <label for="message">Message:</label>
            <textarea id="message" name="message" rows="3" required></textarea>
            
            <button type="submit">Send Message</button>
        </form>
    </div>
    
    <p><a href="/">← Back to Home</a></p>
    
    <script>
        // Only auto-refresh if user is NOT typing
        let lastActivity = Date.now();
        let refreshInterval = 5000; // 5 seconds
        
        // Track when user is typing
        document.addEventListener('input', function() {
            lastActivity = Date.now();
        });
        
        // Track when user focuses on input fields
        document.addEventListener('focusin', function(e) {
            if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA') {
                lastActivity = Date.now();
            }
        });
        
        // Auto-refresh only if idle for 5 seconds
        setInterval(function() {
            let timeSinceActivity = Date.now() - lastActivity;
            if (timeSinceActivity > refreshInterval) {
                location.reload();
            }
        }, 1000); // Check every second
    </script>
</body>
</html>
)";
    
    return html.str();
}

std::string ChatHandler::getCurrentTimestamp() const {
    time_t timestamp;
    time(&timestamp);
    return std::string(ctime(&timestamp));
}
