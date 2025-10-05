#pragma once

#include "duckdb.hpp"

namespace duckdb {

class NotionRead {
public:
    static void RegisterTableFunction(DatabaseInstance &db);
};

} // namespace duckdb
