#pragma once

#include <string>
#include <map>

class HttpResponse {
public:
    HttpResponse(int status_code = 200, const std::string& status_text = "OK");
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);

    // Build full HTTP response
    std::string build() const;

    static HttpResponse ok(const std::string& body = "");
    static HttpResponse html(const std::string& html_content);
    static HttpResponse notFound();
    static HttpResponse badRequest();

private:
    int status_code_;
    std::string status_text_;
    std::map<std::string, std::string> headers_;
    std::string body_;
};
