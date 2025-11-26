#include "HttpServer.h"
#include <iostream>
#include <sstream>

HttpServer::HttpServer(int port) 
    : port_(port), running_(false), connection_count_(0) {
    
    registerRoute("/", [this](const HttpRequest& req) {
        return HttpResponse::html(R"(
            <html>
            <head><title>HTTP Server</title></head>
            <body>
                <h1>Welcome to the HTTP Server</h1>
                <ul>
                    <li><a href="/chat">Go to Chat</a></li>
                    <li><a href="/admin/info">Server Info</a></li>
                </ul>
            </body>
            </html>
        )");
    });
    
    registerRoute("/chat", [this](const HttpRequest& req) {
        return HttpResponse::html(chat_handler_.generateChatPage());
    });
    
    registerRoute("/chat/send", [this](const HttpRequest& req) {
        if (req.getMethod() == HttpRequest::Method::POST) {
            std::string username = parseFormData(req.getBody(), "username");
            std::string message = parseFormData(req.getBody(), "message");
            
            if (!username.empty() && !message.empty()) {
                chat_handler_.addMessage(username, message);
            }
            
            HttpResponse response(303, "See Other");
            response.setHeader("Location", "/chat");
            response.setBody("");
            return response;
        }
        return HttpResponse::badRequest();
    });
    
    registerRoute("/admin/info", [this](const HttpRequest& req) {
        std::ostringstream info;
        info << "HTTP Server Status\n"
             << "==================\n"
             << "Port: " << port_ << "\n"
             << "Total Connections: " << connection_count_ << "\n"
             << "Chat Messages: " << chat_handler_.getMessages().size() << "\n";
        return HttpResponse::ok(info.str());
    });
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
    
    std::cout << "Server running on http://localhost:" << port_ << std::endl;
    std::cout << "Visit http://localhost:" << port_ << "/chat for the chat app" << std::endl;
    
    running_ = true;
    
    while (running_) {
        Socket client_socket = server_socket_.accept();
        
        if (!client_socket.isValid()) {
            continue;
        }
        
        connection_count_++;
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
}