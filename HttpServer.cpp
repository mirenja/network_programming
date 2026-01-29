#include "HttpServer.h"
#include <iostream>
#include <sstream>
#include <memory>

HttpServer::HttpServer(int port) 
    : port_(port), running_(false), connection_count_(0) {
    
    // Initialize state manager
    state_manager_ = std::make_unique<StateManager>("chat_state.dat");
    
    // ChatHandler loads its own state
    chat_handler_.loadState(*state_manager_);
    
    registerRoute("/", [this](const HttpRequest& req) {
        std::string username = getCurrentUser(req);
        // std::string username = "Haven't added session mgmt yet";
        std::ostringstream html;
        
        html << R"(
            <html>
            <head><title>HTTP Server</title></head>
            <body style="font-family: Arial; max-width: 800px; margin: 50px auto; padding: 20px;">
                <h1>Welcome to the HTTP Server</h1>)";
        
        if (!username.empty()) {
            html << "<p>Logged in as: <strong>" << username << "</strong></p>";
        }
        
        html << R"(
                <ul>
                    <li><a href="/chat">Go to Chat</a></li>
                    <li><a href="/admin/info">Server Info</a></li>
                    <li><a href="/logout">Logout</a></li>
                </ul>
            </body>
            </html>
        )";
        
        return HttpResponse::html(html.str());
    });
    
    // Chat page route
    registerRoute("/chat", [this](const HttpRequest& req) {
        std::string username = getCurrentUser(req);
        // std::string username = "Haven't added session mgmt yet";
        return HttpResponse::html(chat_handler_.generateChatPage(username));
    });
    
    // Send message route
   registerRoute("/chat/send", [this](const HttpRequest& req) {
    if (req.getMethod() != HttpRequest::Method::POST) {
        return HttpResponse::badRequest();
    }

    // Message from form
    std::string message = parseFormData(req.getBody(), "message");
    if (message.empty()) {
        return HttpResponse::badRequest();
    }

    // Session management
    std::string cookie_header = req.getHeader("Cookie");
    std::string session_id = session_manager_.parseSessionCookie(cookie_header);

    UserSession* session = nullptr;

    if (!session_id.empty()) {
        session = session_manager_.getSession(session_id);
    }

    // No valid session → create one using username from form
    if (!session) {
        std::string username = parseFormData(req.getBody(), "username");
        if (username.empty()) {
            return HttpResponse::badRequest();
        }

        session_id = session_manager_.createSession(username);
        session = session_manager_.getSession(session_id);
    } else {
        session_manager_.updateActivity(session_id);
    }

    chat_handler_.addMessage(session->username, message);
    session->message_count++;

    saveState();

  
    HttpResponse response(303, "See Other");
    response.setHeader("Location", "/chat");
    response.setHeader("Set-Cookie",session_manager_.generateSetCookieHeader(session_id));
    response.setBody("");
    return response;
});

    
    // Admin info route
    registerRoute("/admin/info", [this](const HttpRequest& req) {
        std::ostringstream info;
        info << "HTTP Server Status\n"
             << "==================\n"
             << "Port: " << port_ << "\n"
             << "Total Connections: " << connection_count_ << "\n"
             << "Chat Messages: " << chat_handler_.getMessages().size() << "\n";
        return HttpResponse::ok(info.str());
    });
    
    // Logout route
    registerRoute("/logout", [this](const HttpRequest& req) {
        std::string cookie_header = req.getHeader("Cookie");
        std::string session_id = session_manager_.parseSessionCookie(cookie_header);

        if (!session_id.empty()) {
            session_manager_.deleteSession(session_id);
        }

        HttpResponse response(303, "See Other");
        response.setHeader("Location", "/");
        response.setHeader(
            "Set-Cookie",
            "session_id=; Path=/; HttpOnly; Max-Age=0"
        );
        response.setBody("");
        return response;
    });
};

HttpServer::~HttpServer() {
    saveState();
}

std::string HttpServer::getCurrentUser(const HttpRequest& request) {
    std::string cookie_header = request.getHeader("Cookie");
    std::string session_id = session_manager_.parseSessionCookie(cookie_header);
    
    if (session_id.empty()) {
        return "";
    }
    
    UserSession* session = session_manager_.getSession(session_id);
    if (session) {
        session_manager_.updateActivity(session_id);
        return session->username;
    }
    
    return "";
}

void HttpServer::saveState() {
    chat_handler_.saveState(*state_manager_);
}

void HttpServer::registerRoute(const std::string& path, RouteHandler handler) {
    routes_[path] = handler;
}

void HttpServer::run() {
    if (!server_socket_.bind(port_)) {
        std::cerr << "Failed to bind to port " << port_ << std::endl;
        return;
    }
    
    if (!server_socket_.listen()) {
        std::cerr << "Failed to listen on port " << port_ << std::endl;
        return;
    }
    std::cout << "[SERVER SOCKET]\n";
    std::cout << "Server running on http://localhost:" << port_ << std::endl;
    std::cout << "Visit http://localhost:" << port_ << "/chat for the chat app" << std::endl;
    
    running_ = true;
    
    while (running_) {
        Socket client_socket = server_socket_.accept();
        
        if (!client_socket.isValid()) {
            continue;
        }
        std::cout << "[CLIENT CONNECTED]\n";
        
        connection_count_++;
        
        // Clean up expired sessions periodically
        if (connection_count_ % 10 == 0) {
            session_manager_.cleanupExpiredSessions();
        }
        
        handleClient(std::move(client_socket));
    }
}

void HttpServer::handleClient(Socket client_socket) {
    std::string raw_request = client_socket.receive();
    
    if (raw_request.empty()) {
        return;
    }
    
    std::cout << "\n=== Request ===" << std::endl;
    std::cout << raw_request.substr(0, 200) << "..." << std::endl;
    
    HttpRequest request;
    if (!request.parse(raw_request)) {
        std::cerr << "Failed to parse request" << std::endl;
        client_socket.send(HttpResponse::badRequest().build());
        return;
    }
    
    HttpResponse response = routeRequest(request);
    
    std::string response_str = response.build();
    client_socket.send(response_str);
    
    std::cout << "Sent " << response_str.size() << " bytes" << std::endl;
}

HttpResponse HttpServer::routeRequest(const HttpRequest& request) {
    std::string path = request.getPath();
    
    auto it = routes_.find(path);
    if (it != routes_.end()) {
        return it->second(request);
    }
    
    return HttpResponse::notFound();
}

std::string HttpServer::parseFormData(const std::string& body, const std::string& key) {
    std::string search = key + "=";
    size_t pos = body.find(search);
    
    if (pos == std::string::npos) {
        return "";
    }
    
    size_t start = pos + search.length();
    size_t end = body.find('&', start);
    
    std::string value = (end == std::string::npos) 
                        ? body.substr(start) 
                        : body.substr(start, end - start);
    
    std::string decoded;
    for (size_t i = 0; i < value.length(); ++i) {
        if (value[i] == '+') {
            decoded += ' ';
        } else if (value[i] == '%' && i + 2 < value.length()) {
            int hex = std::stoi(value.substr(i + 1, 2), nullptr, 16);
            decoded += static_cast<char>(hex);
            i += 2;
        } else {
            decoded += value[i];
        }
    }
    
    return decoded;
}

void HttpServer::stop() {
    running_ = false;
    saveState();
}