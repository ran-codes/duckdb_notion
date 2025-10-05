# DuckDB Notion Extension - Example Notebooks

This directory contains Quarto notebooks demonstrating the DuckDB Notion extension.

## Notebooks

1. **01-getting-started.qmd** - Introduction to the extension, setup, and basic queries
2. **02-data-analysis.qmd** - Advanced analytics, joins, and visualizations
3. **03-etl-pipeline.qmd** - Building ETL pipelines from various sources to Notion

## Prerequisites

```bash
pip install duckdb pandas plotly python-dotenv matplotlib
```

## Setup

Create a `.env` file in the project root:

```
NOTION_TOKEN=secret_your_token_here
NOTION_DATABASE_ID=your_database_id
```

## Running the Notebooks

### Render Individual Notebook

```bash
quarto render _notebooks/01-getting-started.qmd
```

### Render All Notebooks

```bash
quarto render _notebooks/
```

### Preview with Live Reload

```bash
quarto preview _notebooks/01-getting-started.qmd
```

## Output

Rendered HTML files will be created in the same directory. Open them in your browser to view the results.

## Note

These notebooks assume the extension is built and loaded. For testing before the extension is available:

1. Build the extension from source
2. Load it manually in DuckDB
3. Update the notebook code to use your local build

## Examples Included

- ✅ Reading from Notion databases
- ✅ Writing data to Notion
- ✅ Complex analytical queries
- ✅ Joining multiple data sources
- ✅ Time-based analysis
- ✅ ETL pipeline examples
- ✅ Data visualization with Plotly and Matplotlib
