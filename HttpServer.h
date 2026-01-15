#pragma once


#include "Socket.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ChatHandler.h"
#include <functional>
#include <map>
#include <string>
// #include "ThreadPool.h"


class HttpServer {
public:
    using RouteHandler = std::function<HttpResponse(const HttpRequest&)>;
    
    HttpServer(int port);
    
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
    
    void handleClient(Socket client_socket);
    HttpResponse routeRequest(const HttpRequest& request);
    std::string parseFormData(const std::string& body, const std::string& key);
    
};