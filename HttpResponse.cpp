#include "HttpResponse.h"
#include <sstream>

HttpResponse::HttpResponse(int status_code, const std::string& status_text)
    : status_code_(status_code), status_text_(status_text) {}

void HttpResponse::setHeader(const std::string& key, const std::string& value)
{
    headers_[key] = value;
}

void HttpResponse::setBody(const std::string& body) {
    body_ = body;
}


std::string HttpResponse::build() const {
    std::ostringstream response;
    response << "HTTP/1.1 " << status_code_ << " " << status_text_ << "\r\n";

    for (const auto& [key, value] : headers_) {
        response << key << ": " << value << "\r\n";
    }

    response << "Content-Length: " << body_.size() << "\r\n";
    response << "\r\n";
    response << body_;
    return response.str();
}


HttpResponse HttpResponse::ok(const std::string& body) {
    HttpResponse resp(200, "OK");
    resp.setBody(body);
    return resp;
}

HttpResponse HttpResponse::html(const std::string& html_content) {
    HttpResponse resp(200, "OK");
    resp.setHeader("Content-Type", "text/html");
    resp.setBody(html_content);
    return resp;
}

HttpResponse HttpResponse::notFound() {
    HttpResponse resp(404, "Not Found");
    resp.setBody("404 Not Found");
    return resp;
}

HttpResponse HttpResponse::badRequest() {
    HttpResponse resp(400, "Bad Request");
    resp.setBody("400 Bad Request");
    return resp;
}
