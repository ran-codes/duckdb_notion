#include "notion_utils.hpp"
#include <regex>

namespace duckdb {

std::string NotionUtils::ExtractDatabaseId(const std::string &input) {
    // Check if it's already a database ID (32 characters hex)
    std::regex id_regex("^[a-f0-9]{32}$", std::regex::icase);
    if (std::regex_match(input, id_regex)) {
        return input;
    }

    // Extract from Notion URL format
    // https://www.notion.so/workspace/database_id?v=view_id
    std::regex url_regex(R"(notion\.so/(?:[^/]+/)?([a-f0-9]{32}))", std::regex::icase);
    std::smatch match;
    if (std::regex_search(input, match, url_regex)) {
        return match[1].str();
    }

    // Extract from UUID format with dashes
    std::regex uuid_regex("([a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12})", std::regex::icase);
    if (std::regex_search(input, match, uuid_regex)) {
        std::string uuid = match[1].str();
        // Remove dashes
        uuid.erase(std::remove(uuid.begin(), uuid.end(), '-'), uuid.end());
        return uuid;
    }

    // If no pattern matches, return as-is
    return input;
}

bool NotionUtils::IsNotionUrl(const std::string &input) {
    return input.find("notion.so") != std::string::npos;
}

std::string NotionUtils::ParseJsonString(const std::string &json, const std::string &key) {
    // Simple JSON parsing for string values
    std::string search_key = "\"" + key + "\"";
    size_t pos = json.find(search_key);
    if (pos == std::string::npos) {
        return "";
    }

    pos = json.find(":", pos);
    if (pos == std::string::npos) {
        return "";
    }

    pos = json.find("\"", pos);
    if (pos == std::string::npos) {
        return "";
    }
    pos++; // Skip opening quote

    size_t end_pos = json.find("\"", pos);
    if (end_pos == std::string::npos) {
        return "";
    }

    return json.substr(pos, end_pos - pos);
}

} // namespace duckdb
