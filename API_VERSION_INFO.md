# Notion API Version Information

## Current API Version: 2025-09-03

This extension uses the latest Notion API version (2025-09-03) which includes support for multi-source databases.

## Key Features in This Version

### 1. **Multi-Source Database Support**
The extension now supports Notion's multi-source databases, which allow connecting multiple data sources to a single database.

### 2. **Data Source ID Support**
- API calls can include optional `data_source_id` parameter
- Automatically discovered when needed
- Backwards compatible with single-source databases

### 3. **New Endpoints**
- `GET /v1/data_sources` - Get data source information
- Enhanced `/v1/pages` with data_source_id support
- Enhanced `/v1/databases/{id}/query` with data_source_id support

## Backwards Compatibility

### ✅ **Fully Compatible:**
- Single-source databases (most common use case)
- Existing authentication methods
- All read/write operations

### ⚠️ **Requires Update:**
- Multi-source databases require `data_source_id` parameter
- Some webhook events have new structure (if using webhooks)

## Usage Examples

### Basic Usage (Single Source)
```sql
-- Works exactly as before - no changes needed
SELECT * FROM read_notion('database_id');

COPY my_table TO 'database_id' (FORMAT notion);
```

### Multi-Source Database Usage
```sql
-- Extension will automatically handle data_source_id
-- No code changes needed by end users

-- The extension:
-- 1. Detects if database has multiple sources
-- 2. Fetches the appropriate data_source_id
-- 3. Includes it in API calls automatically
```

## API Version Header

All requests include:
```
Notion-Version: 2025-09-03
```

## Migration from Older Versions

If you were using an older version of this extension:

1. **No action required** for single-source databases
2. **Automatic handling** for multi-source databases
3. **Update your integration** if you're using custom API calls

## Changes from Previous Versions

### From 2022-06-28 → 2025-09-03

**Breaking Changes:**
- Multi-source databases now require `data_source_id`
- Some webhook event structures changed

**New Features:**
- Multi-source database support
- Enhanced data source discovery
- Better error messages for data source issues

**Deprecated:**
- None (fully backwards compatible for single-source)

## Implementation Details

### CreatePage with Data Source
```cpp
// Old way (still works for single-source)
CreatePage(database_id, auth_token, properties);

// New way (automatically used for multi-source)
CreatePage(database_id, auth_token, properties, data_source_id);
```

### QueryDatabase with Data Source
```cpp
// Old way (still works)
QueryDatabase(database_id, auth_token, cursor);

// New way (optional parameter)
QueryDatabase(database_id, auth_token, cursor, data_source_id);
```

## Error Handling

### Common Errors:

1. **Missing data_source_id** (multi-source databases)
   ```
   Error: Database has multiple data sources. Please specify data_source_id.
   ```

2. **Invalid data_source_id**
   ```
   Error: Data source not found or not accessible.
   ```

3. **Permission issues**
   ```
   Error: Integration does not have access to data source.
   ```

## Testing Multi-Source Databases

To test multi-source functionality:

1. Create a Notion database
2. Add a second data source to it (via Notion UI)
3. Share both sources with your integration
4. Query the database - extension will handle data sources automatically

## Future API Versions

This extension will be updated to support future Notion API versions as they're released. Check this file for version information.

## References

- [Notion API Upgrade Guide 2025-09-03](https://developers.notion.com/docs/upgrade-guide-2025-09-03)
- [Notion API Reference](https://developers.notion.com/reference/)
- [DuckDB Extension Documentation](./README.md)

## Version History

| Extension Version | Notion API Version | Release Date |
|------------------|-------------------|--------------|
| 0.1.0            | 2022-06-28        | 2024-02-XX   |
| 0.2.0            | 2025-09-03        | 2024-02-XX   |
