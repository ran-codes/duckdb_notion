# DuckDB Notion Extension

A DuckDB extension that enables reading from and writing to Notion databases directly using SQL.

**📚 [View Full Documentation](https://yourusername.github.io/duckdb_notion/)**

🚧 **Experimental** 🚧 This is an experimental community extension.

**API Version:** Notion API 2025-09-03 (Latest)

## Features

- **Read Notion databases** using SQL queries
- **Write data to Notion** databases using COPY TO
- **Token-based authentication** via environment variables or DuckDB secrets
- **Automatic schema detection** from Notion database properties

## Installation

```sql
-- Once published to community extensions
INSTALL notion FROM community;
LOAD notion;
```

## Authentication

### Option 1: Environment Variable

```bash
export NOTION_TOKEN="your_notion_integration_token"
```

### Option 2: DuckDB Secret

```sql
CREATE SECRET notion_secret (
    TYPE notion,
    token 'your_notion_integration_token'
);
```

### Getting a Notion Integration Token

1. Go to [https://www.notion.so/my-integrations](https://www.notion.so/my-integrations)
2. Click "+ New integration"
3. Give it a name and select capabilities
4. Copy the "Internal Integration Token"
5. Share your database with the integration

## Usage

### Reading from Notion

```sql
-- Read from a Notion database using database ID
SELECT * FROM read_notion('database_id');

-- Read from a Notion database using full URL
SELECT * FROM read_notion('https://www.notion.so/workspace/database_id?v=view_id');
```

### Writing to Notion

```sql
-- Create a table
CREATE TABLE my_data (
    name VARCHAR,
    count INTEGER,
    active BOOLEAN
);

-- Insert some data
INSERT INTO my_data VALUES
    ('Item 1', 42, true),
    ('Item 2', 17, false);

-- Copy to Notion database
COPY my_data TO 'database_id' (FORMAT notion);

-- Or use full URL
COPY my_data TO 'https://www.notion.so/workspace/database_id' (FORMAT notion);
```

## Supported Data Types

### Reading from Notion

- **Title**: Mapped to VARCHAR
- **Rich Text**: Mapped to VARCHAR
- **Number**: Mapped to DOUBLE
- **Checkbox**: Mapped to BOOLEAN
- **Select**: Mapped to VARCHAR
- **URL**: Mapped to VARCHAR
- **Date**: Mapped to VARCHAR
- **Created Time**: Mapped to VARCHAR
- **Last Edited Time**: Mapped to VARCHAR

### Writing to Notion

- **VARCHAR**: Written as title (first column) or rich_text
- **INTEGER/BIGINT**: Written as number
- **DOUBLE/FLOAT**: Written as number
- **BOOLEAN**: Written as checkbox

## Examples

### Example 1: Analyzing Notion Database

```sql
-- Load the extension
LOAD notion;

-- Read tasks from a Notion database
SELECT * FROM read_notion('your_database_id');

-- Count tasks by status
SELECT status, COUNT(*) as count
FROM read_notion('your_database_id')
GROUP BY status;
```

### Example 2: Syncing Data to Notion

```sql
-- Read from CSV
CREATE TABLE tasks AS
SELECT * FROM read_csv('tasks.csv');

-- Write to Notion
COPY tasks TO 'your_database_id' (FORMAT notion);
```

### Example 3: Joining Notion with Other Data

```sql
-- Join Notion database with local data
SELECT
    n.name,
    n.status,
    l.additional_info
FROM read_notion('notion_db_id') n
JOIN local_table l ON n.id = l.notion_id;
```

## Building from Source

### Prerequisites

- CMake 3.5 or higher
- C++17 compatible compiler
- OpenSSL
- DuckDB development headers

### Build Steps

```bash
# Clone the repository
git clone https://github.com/yourusername/duckdb_notion.git
cd duckdb_notion

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Install
make install
```

## Architecture

The extension is structured into several components:

- **notion_extension.cpp**: Main extension entry point and initialization
- **notion_auth.cpp**: Authentication handling (secrets and environment variables)
- **notion_requests.cpp**: HTTP/HTTPS communication with Notion API
- **notion_read.cpp**: Table function for reading Notion databases
- **notion_write.cpp**: Copy function for writing to Notion databases
- **notion_utils.cpp**: Utility functions (URL parsing, JSON helpers)

## Limitations

- Currently uses simplified JSON parsing (consider using a proper JSON library for production)
- Property type mapping is basic and may need enhancement for complex types
- Pagination is handled automatically but large databases may take time to query
- Write operations create new pages (no update support yet)

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

MIT License

## Acknowledgments

Inspired by [duckdb_gsheets](https://github.com/evidence-dev/duckdb_gsheets) extension for Google Sheets.
