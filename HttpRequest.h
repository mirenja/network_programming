#pragma once

#include <string>
#include <map>

class HttpRequest {
public:
    HttpRequest();
    enum class Method {GET,POST,PUT,DELETE,UNKNOWN};
    bool parse(const std::string& raw_request);
    
    Method getMethod() const;
    std::string getPath() const;
    std::string getHeader(const std::string& key) const;
    std::string getBody() const;

private:
    Method method_;
    std::string path_;
    std::string version_;
    std::map<std::string, std::string> headers_;
    std::string body_;
    Method parseMethod(const std::string& method_str);
};