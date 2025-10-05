#pragma once

#include "duckdb.hpp"
#include <string>

namespace duckdb {

class NotionUtils {
public:
    static std::string ExtractDatabaseId(const std::string &input);
    static bool IsNotionUrl(const std::string &input);
    static std::string ParseJsonString(const std::string &json, const std::string &key);
};

} // namespace duckdb
