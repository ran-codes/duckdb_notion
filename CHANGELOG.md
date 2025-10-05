# Changelog

All notable changes to the DuckDB Notion extension will be documented in this file.

## [Unreleased]

### Added
- ✅ Complete DuckDB extension for reading and writing Notion databases
- ✅ Support for Notion API version 2025-09-03 (latest)
- ✅ Multi-source database support with `data_source_id`
- ✅ Authentication via environment variables or DuckDB secrets
- ✅ Read operations with `read_notion()` table function
- ✅ Write operations with `COPY TO ... (FORMAT notion)`
- ✅ Automatic pagination for large databases
- ✅ URL parsing for database IDs
- ✅ Property type mapping (VARCHAR, INTEGER, DOUBLE, BOOLEAN)
- ✅ Professional documentation website
- ✅ Example Quarto notebooks for testing
- ✅ Comprehensive test queries and setup guides

### Changed
- 🔄 Updated to Notion API 2025-09-03 from 2022-06-28
- 🔄 Added optional `data_source_id` parameter to API calls
- 🔄 Enhanced CreatePage to support multi-source databases
- 🔄 Enhanced QueryDatabase to support multi-source databases

### Technical Details
- **API Version:** 2025-09-03
- **DuckDB Version:** 1.0.0+
- **C++ Standard:** C++17
- **Dependencies:** OpenSSL

## [0.1.0] - Initial Release

### Core Features
- Basic read functionality from Notion databases
- Basic write functionality to Notion databases
- HTTP/HTTPS request handling with OpenSSL
- Simple JSON parsing for API responses
- Authentication via secrets and environment variables

### Architecture
- `notion_extension.cpp` - Main extension entry point
- `notion_auth.cpp` - Authentication handling
- `notion_requests.cpp` - HTTP API communication
- `notion_read.cpp` - Table function for reading
- `notion_write.cpp` - Copy function for writing
- `notion_utils.cpp` - Utility functions

### Known Limitations
- Simplified JSON parsing (not production-grade)
- Basic property type mapping
- No update operations (only create)
- No filtering/sorting at API level
- No retry logic for rate limits

## Roadmap

### v0.2.0 (Planned)
- [ ] Proper JSON library integration (nlohmann/json or yyjson)
- [ ] Support for complex property types (relations, rollups, formulas)
- [ ] Update operations (PATCH)
- [ ] Filtering and sorting at API level
- [ ] Rate limit handling and retry logic

### v0.3.0 (Planned)
- [ ] Batch operations for better performance
- [ ] Incremental sync support
- [ ] Better error messages and diagnostics
- [ ] Performance optimizations
- [ ] Webhook support

### v1.0.0 (Future)
- [ ] Production-ready JSON parsing
- [ ] Full property type support
- [ ] Advanced query optimization
- [ ] Complete test coverage
- [ ] Performance benchmarks
- [ ] Official DuckDB community extension release

## Breaking Changes

None yet - this is the initial release.

## Migration Guide

Not applicable - initial release.

## Credits

- Inspired by [duckdb_gsheets](https://github.com/evidence-dev/duckdb_gsheets)
- Built with [DuckDB Extension Template](https://github.com/duckdb/extension-template)
- Uses [Notion API](https://developers.notion.com/)

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

MIT License - see [LICENSE](LICENSE) for details.
