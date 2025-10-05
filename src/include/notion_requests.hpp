#pragma once

#include "duckdb.hpp"
#include <string>

namespace duckdb {

struct NotionResponse {
    std::string body;
    int status_code;
    bool success;
};

class NotionRequests {
public:
    static NotionResponse QueryDatabase(const std::string &database_id,
                                        const std::string &auth_token,
                                        const std::string &start_cursor = "",
                                        const std::string &data_source_id = "");

    static NotionResponse GetDatabase(const std::string &database_id,
                                      const std::string &auth_token);

    static NotionResponse GetDataSource(const std::string &database_id,
                                       const std::string &auth_token);

    static NotionResponse CreatePage(const std::string &database_id,
                                     const std::string &auth_token,
                                     const std::string &properties,
                                     const std::string &data_source_id = "");

    static NotionResponse UpdatePage(const std::string &page_id,
                                     const std::string &auth_token,
                                     const std::string &properties);

private:
    static NotionResponse MakeRequest(const std::string &url,
                                      const std::string &auth_token,
                                      const std::string &method = "GET",
                                      const std::string &body = "");
};

} // namespace duckdb
