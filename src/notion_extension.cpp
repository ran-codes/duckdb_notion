#include "notion_extension.hpp"
#include "notion_auth.hpp"
#include "notion_read.hpp"
#include "notion_write.hpp"
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace duckdb {

void NotionExtension::Load(DuckDB &db) {
    auto &db_instance = *db.instance;

    // Initialize OpenSSL
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();

    // Register authentication/secret functions
    NotionAuth::RegisterSecretFunctions(db_instance);

    // Register table functions for reading
    NotionRead::RegisterTableFunction(db_instance);

    // Register copy function for writing
    NotionWrite::RegisterCopyFunction(db_instance);
}

std::string NotionExtension::Name() {
    return "notion";
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void notion_init(duckdb::DatabaseInstance &db) {
    duckdb::DuckDB db_wrapper(db);
    db_wrapper.LoadExtension<duckdb::NotionExtension>();
}

DUCKDB_EXTENSION_API const char *notion_version() {
    return duckdb::DuckDB::LibraryVersion();
}

}
