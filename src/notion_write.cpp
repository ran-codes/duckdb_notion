#include "notion_write.hpp"
#include "notion_auth.hpp"
#include "notion_requests.hpp"
#include "notion_utils.hpp"
#include "duckdb/main/extension_util.hpp"
#include "duckdb/common/exception.hpp"
#include <sstream>

namespace duckdb {

struct NotionCopyData : public TableFunctionData {
    std::string database_id;
    std::string auth_token;
};

struct NotionCopyGlobalState : public GlobalTableFunctionState {
    idx_t rows_written = 0;

    idx_t MaxThreads() const override {
        return 1;
    }
};

// Helper to convert a row to Notion properties JSON
static std::string RowToNotionProperties(const vector<string> &column_names,
                                        const vector<LogicalType> &column_types,
                                        DataChunk &chunk,
                                        idx_t row_idx) {
    std::stringstream properties;
    properties << "{";

    for (idx_t col_idx = 0; col_idx < column_names.size(); col_idx++) {
        if (col_idx > 0) {
            properties << ",";
        }

        const auto &name = column_names[col_idx];
        const auto &type = column_types[col_idx];
        auto value = chunk.GetValue(col_idx, row_idx);

        properties << "\"" << name << "\":";

        if (value.IsNull()) {
            // For title properties, we need at least an empty array
            if (col_idx == 0) {
                properties << "{\"title\":[]}";
            } else {
                properties << "null";
            }
        } else {
            // Determine Notion property type based on DuckDB type
            switch (type.id()) {
                case LogicalTypeId::VARCHAR: {
                    // Assume first column is title, others are rich_text
                    if (col_idx == 0) {
                        properties << "{\"title\":[{\"text\":{\"content\":\""
                                  << value.ToString() << "\"}}]}";
                    } else {
                        properties << "{\"rich_text\":[{\"text\":{\"content\":\""
                                  << value.ToString() << "\"}}]}";
                    }
                    break;
                }
                case LogicalTypeId::INTEGER:
                case LogicalTypeId::BIGINT:
                case LogicalTypeId::DOUBLE:
                case LogicalTypeId::FLOAT: {
                    properties << "{\"number\":" << value.ToString() << "}";
                    break;
                }
                case LogicalTypeId::BOOLEAN: {
                    properties << "{\"checkbox\":" << (value.GetValue<bool>() ? "true" : "false") << "}";
                    break;
                }
                default: {
                    // Default to rich_text for unknown types
                    properties << "{\"rich_text\":[{\"text\":{\"content\":\""
                              << value.ToString() << "\"}}]}";
                    break;
                }
            }
        }
    }

    properties << "}";
    return properties.str();
}

static unique_ptr<FunctionData> NotionCopyBind(ClientContext &context, CopyFunctionBindInput &input,
                                               const vector<string> &column_names,
                                               const vector<LogicalType> &column_types) {
    auto bind_data = make_uniq<NotionCopyData>();

    // Extract database ID from file path (which is the destination)
    bind_data->database_id = NotionUtils::ExtractDatabaseId(input.file_path);

    // Get auth token
    bind_data->auth_token = NotionAuth::GetAuthToken(context);

    return std::move(bind_data);
}

static unique_ptr<GlobalFunctionData> NotionCopyInit(ClientContext &context, FunctionData &bind_data_p) {
    auto state = make_uniq<NotionCopyGlobalState>();
    return std::move(state);
}

static void NotionCopySink(ExecutionContext &context, FunctionData &bind_data_p, GlobalFunctionData &gstate_p,
                          LocalFunctionData &lstate, DataChunk &input) {
    auto &bind_data = bind_data_p.Cast<NotionCopyData>();
    auto &gstate = gstate_p.Cast<NotionCopyGlobalState>();

    // Get column information from the input chunk
    vector<string> column_names;
    vector<LogicalType> column_types;

    for (idx_t i = 0; i < input.ColumnCount(); i++) {
        column_names.push_back("Column" + std::to_string(i));
        column_types.push_back(input.data[i].GetType());
    }

    // Write each row to Notion
    for (idx_t row_idx = 0; row_idx < input.size(); row_idx++) {
        std::string properties = RowToNotionProperties(column_names, column_types, input, row_idx);

        auto response = NotionRequests::CreatePage(bind_data.database_id,
                                                   bind_data.auth_token,
                                                   properties);

        if (!response.success) {
            throw IOException("Failed to write row to Notion: " + response.body);
        }

        gstate.rows_written++;
    }
}

static void NotionCopyFinalize(ClientContext &context, FunctionData &bind_data_p, GlobalFunctionData &gstate_p) {
    auto &gstate = gstate_p.Cast<NotionCopyGlobalState>();
    // Could log the number of rows written here
}

void NotionWrite::RegisterCopyFunction(DatabaseInstance &db) {
    CopyFunction notion_copy("notion");
    notion_copy.copy_to_bind = NotionCopyBind;
    notion_copy.copy_to_initialize_global = NotionCopyInit;
    notion_copy.copy_to_sink = NotionCopySink;
    notion_copy.copy_to_finalize = NotionCopyFinalize;

    notion_copy.extension = "notion";

    ExtensionUtil::RegisterFunction(db, notion_copy);
}

} // namespace duckdb
