#pragma once


#include "Socket.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ChatHandler.h"
#include <functional>
#include <map>
#include <string>
#include "StateManager.h"
#include "SessionManager.h"
// #include "ThreadPool.h"


class HttpServer {
public:
    using RouteHandler = std::function<HttpResponse(const HttpRequest&)>;
    
    HttpServer(int port);
    ~HttpServer();
    
    void registerRoute(const std::string& path, RouteHandler handler);
    void run();
    void stop();
    
private:
    int port_;
    Socket server_socket_;
    bool running_;
    int connection_count_;
    
    // ThreadPool pool_;
    std::map<std::string, RouteHandler> routes_;
    ChatHandler chat_handler_;
    std::unique_ptr<StateManager> state_manager_;
    SessionManager session_manager_;
    
    void handleClient(Socket client_socket);
    HttpResponse routeRequest(const HttpRequest& request);
    std::string parseFormData(const std::string& body, const std::string& key);

    std::string getCurrentUser(const HttpRequest& request);
    void saveState();
    
};