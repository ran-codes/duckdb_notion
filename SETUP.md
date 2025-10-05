# Setup Instructions

## Documentation Website

The project includes a professional documentation website located in the `/docs` directory.

### Local Preview

To preview the documentation site locally:

1. Open `docs/index.html` in your browser
2. Or use a local server:
   ```bash
   cd docs
   python -m http.server 8000
   # Visit http://localhost:8000
   ```

### GitHub Pages Deployment

The documentation site will automatically deploy to GitHub Pages when you push to the main branch.

#### Setup Steps:

1. **Enable GitHub Pages**:
   - Go to your repository settings
   - Navigate to "Pages" section
   - Source: GitHub Actions

2. **Push your code**:
   ```bash
   git add .
   git commit -m "Add DuckDB Notion extension with documentation"
   git push origin main
   ```

3. **Access your site**:
   - GitHub Pages URL: `https://yourusername.github.io/duckdb_notion/`
   - Custom domain (optional): See below

### Custom Domain Setup

If you own `duckdb-notion.com` (or similar):

1. Add DNS records:
   ```
   A     @     185.199.108.153
   A     @     185.199.109.153
   A     @     185.199.110.153
   A     @     185.199.111.153
   CNAME www   yourusername.github.io
   ```

2. Update `docs/CNAME` with your domain

3. In GitHub Settings > Pages, add your custom domain

## Building the Extension

### Prerequisites

Install the required dependencies:

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git libssl-dev
```

**macOS:**
```bash
brew install cmake openssl
```

**Windows (MSYS2):**
```bash
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc mingw-w64-x86_64-openssl
```

### Build Steps

1. **Clone DuckDB** (needed for headers):
   ```bash
   git clone https://github.com/duckdb/duckdb.git
   cd duckdb
   make
   cd ..
   ```

2. **Build the extension**:
   ```bash
   cd duckdb_notion
   mkdir build && cd build
   cmake -DDUCKDB_PATH=../../duckdb ..
   make
   ```

3. **Test the extension**:
   ```bash
   cd ../../duckdb
   ./build/release/duckdb
   ```

   In DuckDB:
   ```sql
   LOAD '../duckdb_notion/build/notion.duckdb_extension';

   -- Set your token
   SET VARIABLE notion_token = 'secret_...';

   -- Test reading
   SELECT * FROM read_notion('your_database_id') LIMIT 5;
   ```

## Getting Started with Notion API

1. **Create a Notion Integration**:
   - Visit https://www.notion.so/my-integrations
   - Click "+ New integration"
   - Name it (e.g., "DuckDB Connector")
   - Select capabilities: Read content, Update content, Insert content
   - Copy the "Internal Integration Token"

2. **Share a database with your integration**:
   - Open a Notion database
   - Click "..." menu → "Add connections"
   - Select your integration

3. **Get the database ID**:
   - Open the database in Notion
   - Copy the URL: `https://notion.so/workspace/DATABASE_ID?v=...`
   - The `DATABASE_ID` is the 32-character hex string

4. **Test the connection**:
   ```sql
   SELECT * FROM read_notion('DATABASE_ID');
   ```

## Marketing the Extension

### Key Selling Points

1. **Unique Value Proposition**:
   - Only SQL interface to Notion databases
   - Combines Notion's flexibility with DuckDB's analytical power
   - No Python/JavaScript required - pure SQL

2. **Target Audience**:
   - Data analysts using Notion for project management
   - Teams tracking metrics in Notion
   - Developers building data pipelines
   - Business intelligence professionals

3. **Use Cases to Highlight**:
   - Automated reporting from Notion data
   - Syncing Notion with data warehouses
   - Complex analytics on Notion databases
   - Joining Notion data with other sources

### Distribution Channels

1. **DuckDB Community Extensions**:
   - Submit to https://github.com/duckdb/community-extensions
   - Follow their submission guidelines

2. **Social Media**:
   - Tweet with #DuckDB hashtag
   - Share on r/dataengineering, r/notion
   - LinkedIn posts in data communities

3. **Blog Posts**:
   - "Analyzing Notion Data with SQL"
   - "Building a Notion BI Dashboard with DuckDB"
   - "From Notion to Data Warehouse in One Line of SQL"

4. **Video Content**:
   - YouTube tutorial on setup
   - Demo of common use cases
   - Comparison with Notion API Python libraries

### SEO Keywords

- duckdb notion extension
- notion sql query
- analyze notion database
- notion data export sql
- duckdb notion integration

## Next Steps

1. ✅ Extension code complete
2. ✅ Documentation site ready
3. 🔄 Test with real Notion databases
4. 🔄 Submit to DuckDB community extensions
5. 🔄 Create demo video
6. 🔄 Write blog post
7. 🔄 Share on social media
