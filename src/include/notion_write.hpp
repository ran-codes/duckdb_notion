#pragma once

#include "duckdb.hpp"

namespace duckdb {

class NotionWrite {
public:
    static void RegisterCopyFunction(DatabaseInstance &db);
};

} // namespace duckdb
