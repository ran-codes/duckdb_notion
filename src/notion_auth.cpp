#include "notion_auth.hpp"
#include "duckdb/main/secret/secret_manager.hpp"
#include "duckdb/main/extension_util.hpp"

namespace duckdb {

std::string NotionAuth::GetAuthToken(ClientContext &context) {
    // Try to get token from secret manager
    auto &secret_manager = SecretManager::Get(context);

    try {
        auto transaction = CatalogTransaction::GetSystemCatalogTransaction(context);
        auto secret_match = secret_manager.LookupSecret(transaction, "notion", "notion");

        if (secret_match.HasMatch()) {
            const auto &kv_secret = dynamic_cast<const KeyValueSecret&>(*secret_match.secret_entry->secret);
            Value token_value;
            if (kv_secret.TryGetValue("token", token_value)) {
                return token_value.ToString();
            }
        }
    } catch (...) {
        // Secret not found, will check environment variable
    }

    // Fallback to environment variable
    const char* env_token = std::getenv("NOTION_TOKEN");
    if (env_token) {
        return std::string(env_token);
    }

    throw InvalidInputException("No Notion authentication token found. "
                               "Please set NOTION_TOKEN environment variable or create a secret.");
}

void NotionAuth::RegisterSecretFunctions(DatabaseInstance &db) {
    // Register secret type for Notion
    SecretType secret_type;
    secret_type.name = "notion";
    secret_type.deserializer = KeyValueSecret::Deserialize;
    secret_type.default_provider = "config";

    ExtensionUtil::RegisterSecretType(db, secret_type);

    // Create a create secret function
    auto create_secret_function = [](ClientContext &context, CreateSecretInput &input) -> unique_ptr<BaseSecret> {
        auto scope = input.scope;
        if (scope.empty()) {
            scope = {"notion"};
        }

        // Create key-value secret
        auto secret = make_uniq<KeyValueSecret>(scope, input.type, input.provider, input.name);

        // Add token
        for (const auto &named_param : input.options) {
            if (named_param.first == "token") {
                secret->secret_map["token"] = named_param.second;
            }
        }

        return std::move(secret);
    };

    ExtensionUtil::RegisterSecretFunction(db, "notion", create_secret_function);
}

} // namespace duckdb
