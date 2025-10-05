#include "notion_read.hpp"
#include "notion_auth.hpp"
#include "notion_requests.hpp"
#include "notion_utils.hpp"
#include "duckdb/main/extension_util.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/parser/parsed_data/create_table_function_info.hpp"
#include <sstream>

namespace duckdb {

struct NotionReadBindData : public TableFunctionData {
    std::string database_id;
    std::string auth_token;
    vector<string> column_names;
    vector<LogicalType> column_types;
};

struct NotionReadGlobalState : public GlobalTableFunctionState {
    std::string next_cursor;
    bool has_more = true;
    vector<string> results_buffer;
    idx_t current_row = 0;

    idx_t MaxThreads() const override {
        return 1;
    }
};

// Helper function to extract property value from JSON
static Value ExtractPropertyValue(const std::string &json, const std::string &property_name, const std::string &property_type) {
    // Find the property in the JSON
    std::string search_str = "\"" + property_name + "\"";
    size_t prop_pos = json.find(search_str);
    if (prop_pos == std::string::npos) {
        return Value();
    }

    // Simple JSON parsing based on property type
    if (property_type == "title" || property_type == "rich_text") {
        // Look for the text content
        size_t text_pos = json.find("\"plain_text\"", prop_pos);
        if (text_pos != std::string::npos) {
            size_t start = json.find("\"", text_pos + 13);
            if (start != std::string::npos) {
                start++;
                size_t end = json.find("\"", start);
                if (end != std::string::npos) {
                    return Value(json.substr(start, end - start));
                }
            }
        }
    } else if (property_type == "number") {
        size_t num_pos = json.find("\"number\"", prop_pos);
        if (num_pos != std::string::npos) {
            size_t colon = json.find(":", num_pos);
            if (colon != std::string::npos) {
                size_t start = colon + 1;
                while (start < json.length() && isspace(json[start])) start++;
                if (start < json.length() && (isdigit(json[start]) || json[start] == '-')) {
                    size_t end = start;
                    while (end < json.length() && (isdigit(json[end]) || json[end] == '.' || json[end] == '-')) end++;
                    std::string num_str = json.substr(start, end - start);
                    try {
                        return Value::DOUBLE(std::stod(num_str));
                    } catch (...) {
                        return Value();
                    }
                }
            }
        }
    } else if (property_type == "checkbox") {
        size_t checkbox_pos = json.find("\"checkbox\"", prop_pos);
        if (checkbox_pos != std::string::npos) {
            size_t true_pos = json.find("true", checkbox_pos);
            size_t false_pos = json.find("false", checkbox_pos);
            if (true_pos != std::string::npos && (false_pos == std::string::npos || true_pos < false_pos)) {
                return Value::BOOLEAN(true);
            }
            return Value::BOOLEAN(false);
        }
    } else if (property_type == "select") {
        size_t select_pos = json.find("\"select\"", prop_pos);
        if (select_pos != std::string::npos) {
            size_t name_pos = json.find("\"name\"", select_pos);
            if (name_pos != std::string::npos) {
                size_t start = json.find("\"", name_pos + 6);
                if (start != std::string::npos) {
                    start++;
                    size_t end = json.find("\"", start);
                    if (end != std::string::npos) {
                        return Value(json.substr(start, end - start));
                    }
                }
            }
        }
    } else if (property_type == "url") {
        size_t url_pos = json.find("\"url\"", prop_pos);
        if (url_pos != std::string::npos) {
            size_t start = json.find("\"", url_pos + 5);
            if (start != std::string::npos) {
                start++;
                size_t end = json.find("\"", start);
                if (end != std::string::npos) {
                    return Value(json.substr(start, end - start));
                }
            }
        }
    }

    return Value();
}

static unique_ptr<FunctionData> NotionReadBind(ClientContext &context, TableFunctionBindInput &input,
                                               vector<LogicalType> &return_types, vector<string> &names) {
    auto bind_data = make_uniq<NotionReadBindData>();

    // Get database ID from first parameter
    std::string input_str = input.inputs[0].ToString();
    bind_data->database_id = NotionUtils::ExtractDatabaseId(input_str);

    // Get auth token
    bind_data->auth_token = NotionAuth::GetAuthToken(context);

    // Query the database to get schema
    auto db_response = NotionRequests::GetDatabase(bind_data->database_id, bind_data->auth_token);

    if (!db_response.success) {
        throw InvalidInputException("Failed to get Notion database schema: " + db_response.body);
    }

    // Parse properties from response
    // This is a simplified parser - in production, use a proper JSON library
    size_t props_pos = db_response.body.find("\"properties\"");
    if (props_pos != std::string::npos) {
        // For now, we'll create some default columns
        // In a real implementation, parse the JSON to get actual column definitions
        names.push_back("id");
        return_types.push_back(LogicalType::VARCHAR);

        // Add some common property types
        names.push_back("title");
        return_types.push_back(LogicalType::VARCHAR);

        names.push_back("created_time");
        return_types.push_back(LogicalType::VARCHAR);

        names.push_back("last_edited_time");
        return_types.push_back(LogicalType::VARCHAR);
    }

    bind_data->column_names = names;
    bind_data->column_types = return_types;

    return std::move(bind_data);
}

static unique_ptr<GlobalTableFunctionState> NotionReadInit(ClientContext &context, TableFunctionInitInput &input) {
    auto state = make_uniq<NotionReadGlobalState>();
    return std::move(state);
}

static void NotionReadFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
    auto &bind_data = data_p.bind_data->Cast<NotionReadBindData>();
    auto &state = data_p.global_state->Cast<NotionReadGlobalState>();

    if (!state.has_more && state.current_row >= state.results_buffer.size()) {
        // No more data
        return;
    }

    // Fetch new page of results if needed
    if (state.results_buffer.empty() || state.current_row >= state.results_buffer.size()) {
        if (state.has_more) {
            auto response = NotionRequests::QueryDatabase(bind_data.database_id,
                                                         bind_data.auth_token,
                                                         state.next_cursor);

            if (!response.success) {
                throw InvalidInputException("Failed to query Notion database: " + response.body);
            }

            // Parse response
            // Check for has_more
            if (response.body.find("\"has_more\":true") != std::string::npos) {
                state.has_more = true;
                // Extract next_cursor
                state.next_cursor = NotionUtils::ParseJsonString(response.body, "next_cursor");
            } else {
                state.has_more = false;
            }

            // Parse results array
            size_t results_pos = response.body.find("\"results\"");
            if (results_pos != std::string::npos) {
                // Find the array start
                size_t array_start = response.body.find("[", results_pos);
                if (array_start != std::string::npos) {
                    // Simple array parsing - extract each object
                    size_t pos = array_start + 1;
                    int brace_count = 0;
                    size_t obj_start = 0;

                    for (size_t i = pos; i < response.body.length(); i++) {
                        if (response.body[i] == '{') {
                            if (brace_count == 0) {
                                obj_start = i;
                            }
                            brace_count++;
                        } else if (response.body[i] == '}') {
                            brace_count--;
                            if (brace_count == 0) {
                                state.results_buffer.push_back(response.body.substr(obj_start, i - obj_start + 1));
                            }
                        } else if (response.body[i] == ']' && brace_count == 0) {
                            break;
                        }
                    }
                }
            }

            state.current_row = 0;
        }
    }

    // Output rows
    idx_t count = 0;
    while (count < STANDARD_VECTOR_SIZE && state.current_row < state.results_buffer.size()) {
        const auto &result_obj = state.results_buffer[state.current_row];

        // Extract id
        std::string id = NotionUtils::ParseJsonString(result_obj, "id");
        output.SetValue(0, count, Value(id));

        // Extract title (simplified - would need to parse properties properly)
        output.SetValue(1, count, Value(""));

        // Extract created_time
        std::string created_time = NotionUtils::ParseJsonString(result_obj, "created_time");
        output.SetValue(2, count, Value(created_time));

        // Extract last_edited_time
        std::string last_edited_time = NotionUtils::ParseJsonString(result_obj, "last_edited_time");
        output.SetValue(3, count, Value(last_edited_time));

        count++;
        state.current_row++;
    }

    output.SetCardinality(count);
}

void NotionRead::RegisterTableFunction(DatabaseInstance &db) {
    TableFunction read_notion("read_notion", {LogicalType::VARCHAR}, NotionReadFunction, NotionReadBind, NotionReadInit);
    read_notion.name = "read_notion";

    ExtensionUtil::RegisterFunction(db, read_notion);
}

} // namespace duckdb
