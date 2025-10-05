#include "notion_requests.hpp"
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <sstream>

namespace duckdb {

static const std::string NOTION_API_BASE = "https://api.notion.com/v1";
static const std::string NOTION_VERSION = "2025-09-03";

NotionResponse NotionRequests::MakeRequest(const std::string &url,
                                           const std::string &auth_token,
                                           const std::string &method,
                                           const std::string &body) {
    NotionResponse response;
    response.success = false;
    response.status_code = 0;

    // Parse URL to extract host and path
    std::string host = "api.notion.com";
    std::string path = url;
    if (url.find("https://") == 0) {
        size_t host_start = 8; // After "https://"
        size_t path_start = url.find("/", host_start);
        if (path_start != std::string::npos) {
            host = url.substr(host_start, path_start - host_start);
            path = url.substr(path_start);
        }
    }

    // Initialize SSL context
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        response.body = "Failed to create SSL context";
        return response;
    }

    // Create BIO connection
    BIO *bio = BIO_new_ssl_connect(ctx);
    if (!bio) {
        SSL_CTX_free(ctx);
        response.body = "Failed to create BIO";
        return response;
    }

    // Set up connection
    BIO_set_conn_hostname(bio, (host + ":443").c_str());

    // Get SSL pointer
    SSL *ssl;
    BIO_get_ssl(bio, &ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    // Connect
    if (BIO_do_connect(bio) <= 0) {
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        response.body = "Failed to connect to Notion API";
        return response;
    }

    // Prepare HTTP request
    std::stringstream request;
    request << method << " " << path << " HTTP/1.1\r\n";
    request << "Host: " << host << "\r\n";
    request << "Authorization: Bearer " << auth_token << "\r\n";
    request << "Notion-Version: " << NOTION_VERSION << "\r\n";
    request << "Content-Type: application/json\r\n";
    if (!body.empty()) {
        request << "Content-Length: " << body.length() << "\r\n";
    }
    request << "Connection: close\r\n";
    request << "\r\n";
    if (!body.empty()) {
        request << body;
    }

    std::string request_str = request.str();

    // Send request
    if (BIO_write(bio, request_str.c_str(), request_str.length()) <= 0) {
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        response.body = "Failed to send request";
        return response;
    }

    // Read response
    char buffer[4096];
    std::stringstream response_stream;
    int bytes_read;
    while ((bytes_read = BIO_read(bio, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        response_stream << buffer;
    }

    // Clean up
    BIO_free_all(bio);
    SSL_CTX_free(ctx);

    std::string full_response = response_stream.str();

    // Parse HTTP response
    size_t header_end = full_response.find("\r\n\r\n");
    if (header_end != std::string::npos) {
        std::string headers = full_response.substr(0, header_end);
        response.body = full_response.substr(header_end + 4);

        // Extract status code
        size_t status_pos = headers.find(" ");
        if (status_pos != std::string::npos) {
            std::string status_str = headers.substr(status_pos + 1, 3);
            response.status_code = std::stoi(status_str);
            response.success = (response.status_code >= 200 && response.status_code < 300);
        }
    } else {
        response.body = full_response;
    }

    return response;
}

NotionResponse NotionRequests::QueryDatabase(const std::string &database_id,
                                             const std::string &auth_token,
                                             const std::string &start_cursor,
                                             const std::string &data_source_id) {
    std::string url = NOTION_API_BASE + "/databases/" + database_id + "/query";

    std::stringstream body;
    body << "{";

    bool has_params = false;

    if (!start_cursor.empty()) {
        body << "\"start_cursor\":\"" << start_cursor << "\"";
        has_params = true;
    }

    if (!data_source_id.empty()) {
        if (has_params) body << ",";
        body << "\"data_source_id\":\"" << data_source_id << "\"";
    }

    body << "}";

    return MakeRequest(url, auth_token, "POST", body.str());
}

NotionResponse NotionRequests::GetDatabase(const std::string &database_id,
                                          const std::string &auth_token) {
    std::string url = NOTION_API_BASE + "/databases/" + database_id;
    return MakeRequest(url, auth_token, "GET");
}

NotionResponse NotionRequests::CreatePage(const std::string &database_id,
                                         const std::string &auth_token,
                                         const std::string &properties,
                                         const std::string &data_source_id) {
    std::string url = NOTION_API_BASE + "/pages";

    std::stringstream body;
    body << "{\"parent\":{\"database_id\":\"" << database_id << "\"";

    if (!data_source_id.empty()) {
        body << ",\"data_source_id\":\"" << data_source_id << "\"";
    }

    body << "},\"properties\":" << properties << "}";

    return MakeRequest(url, auth_token, "POST", body.str());
}

NotionResponse NotionRequests::UpdatePage(const std::string &page_id,
                                         const std::string &auth_token,
                                         const std::string &properties) {
    std::string url = NOTION_API_BASE + "/pages/" + page_id;

    std::stringstream body;
    body << "{\"properties\":" << properties << "}";

    return MakeRequest(url, auth_token, "PATCH", body.str());
}

NotionResponse NotionRequests::GetDataSource(const std::string &database_id,
                                            const std::string &auth_token) {
    std::string url = NOTION_API_BASE + "/data_sources?database_id=" + database_id;
    return MakeRequest(url, auth_token, "GET");
}

} // namespace duckdb
