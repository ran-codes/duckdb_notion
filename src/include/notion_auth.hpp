#pragma once

#include "duckdb.hpp"
#include <string>

namespace duckdb {

class NotionAuth {
public:
    static std::string GetAuthToken(ClientContext &context);
    static void RegisterSecretFunctions(DatabaseInstance &db);
};

} // namespace duckdb
