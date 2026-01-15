#include "HttpRequest.h"
#include <sstream>
#include <iostream>

HttpRequest::HttpRequest() : method_(Method::UNKNOWN) {}


HttpRequest::Method HttpRequest::getMethod() const 
{ 
    return method_; 
}
std::string HttpRequest::getPath() const 
{ 
    return path_; 
}
std::string HttpRequest::getBody() const 
{ 
    return body_; 
}


bool HttpRequest::parse(const std::string& raw_request) {
    std::istringstream stream(raw_request);
    std::string line;

    if (!std::getline(stream, line)) 
    return false;
    
    if (!line.empty() && line.back() == '\r') line.pop_back();

    std::istringstream request_line(line);
    std::string method_str;
    request_line >> method_str >> path_ >> version_;

    method_ = parseMethod(method_str);


    while (std::getline(stream, line) && line != "\r") {
        if (line.empty()) break;
        if (!line.empty() && line.back() == '\r') line.pop_back();

        //if colon is not found line.find return npos =-1
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {

            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);

            //strip any spaces or tabs Host: localhost:8080

            size_t start = value.find_first_not_of(" \t");
            if (start != std::string::npos) 
            value = value.substr(start);
            headers_[key] = value;
        }
    }


    body_ = std::string((std::istreambuf_iterator<char>(stream)),
                        std::istreambuf_iterator<char>());

    return true;
}

HttpRequest::Method HttpRequest::parseMethod(const std::string& method_str) 
{
    if (method_str == "GET") return Method::GET;
    if (method_str == "POST") return Method::POST;
    if (method_str == "PUT") return Method::PUT;
    if (method_str == "DELETE") return Method::DELETE;
        return Method::UNKNOWN;
}


std::string HttpRequest::getHeader(const std::string& key) const 
{
    //it is a dictionary
    auto header_value = headers_.find(key);

    // if the key value doesnt exist we dont get an out of range error

    if (header_value != headers_.end()) {
        return header_value -> second;
    } else {
        return "";
    }
}
