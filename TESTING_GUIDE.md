# Testing Guide for DuckDB Notion Extension

## 🍩 Quick Start: Donut Shop Database Example

This guide uses a simple **donut shop business** example to test the extension. It's easy to understand and demonstrates all features including multi-source database support!

### Two Testing Approaches

#### **Approach 1: Single-Source (Traditional) - Recommended for Beginners**
Create 3 separate Notion databases. This is the traditional approach and easier to set up.

#### **Approach 2: Multi-Source (Advanced) - Tests New API Features**
Create 1 Notion database with 3 data sources connected. This demonstrates the 2025-09-03 API multi-source capabilities.

---

## 📋 Setup Option 1: Three Separate Databases (Traditional)

### Database 1: Products 🍩

Track your donut products and inventory.

| Property | Type | Options/Notes |
|----------|------|---------------|
| Name | Title | Product name |
| Category | Select | Classic, Filled, Specialty, Seasonal |
| Price | Number | Selling price in dollars |
| Cost | Number | Cost to make |
| In Stock | Checkbox | Available today? |
| Daily Limit | Number | How many made per day |
| Calories | Number | Nutritional info |

**Sample Data:**
```
Row 1: Glazed Classic | Classic | 2.50 | 0.75 | ✓ | 200 | 250
Row 2: Chocolate Frosted | Classic | 2.75 | 0.85 | ✓ | 150 | 280
Row 3: Jelly Filled | Filled | 3.00 | 1.00 | ✓ | 100 | 320
Row 4: Boston Cream | Filled | 3.25 | 1.20 | ✓ | 80 | 340
Row 5: Pumpkin Spice | Seasonal | 3.50 | 1.30 | ✗ | 0 | 310
Row 6: Maple Bacon | Specialty | 4.00 | 1.50 | ✓ | 50 | 380
```

### Database 2: Orders 🛒

Track customer orders (header information).

| Property | Type | Options/Notes |
|----------|------|---------------|
| Order ID | Title | e.g., "ORD-001" |
| Customer Name | Text | Customer's name |
| Order Date | Date | When ordered |
| Completed | Checkbox | Order fulfilled? |
| Total Amount | Number | Sum of all items |
| Payment Method | Select | Cash, Credit Card, Mobile Pay |
| Order Status | Select | Pending, Processing, Complete, Cancelled |

**Sample Data:**
```
Row 1: ORD-001 | John Smith | 2024-02-15 | ✓ | 21.00 | Credit Card | Complete
Row 2: ORD-002 | Jane Doe | 2024-02-15 | ✓ | 11.00 | Cash | Complete
Row 3: ORD-003 | Bob Wilson | 2024-02-15 | ✓ | 12.00 | Mobile Pay | Complete
Row 4: ORD-004 | Alice Brown | 2024-02-16 | ✓ | 15.00 | Credit Card | Complete
Row 5: ORD-005 | Coffee Shop | 2024-02-16 | ✓ | 41.50 | Credit Card | Complete
Row 6: ORD-006 | Charlie Davis | 2024-02-16 | ✗ | 18.75 | Cash | Pending
```

### Database 3: Order Items 📝

Track individual items within each order (line items).

| Property | Type | Options/Notes |
|----------|------|---------------|
| Item ID | Title | e.g., "ITEM-001" |
| Order | Relation | → Link to Orders database |
| Product | Relation | → Link to Products database |
| Quantity | Number | Number of this product |
| Unit Price | Number | Price at time of order |
| Line Total | Number | Quantity × Unit Price |

**Sample Data:**
```
Row 1: ITEM-001 | ORD-001 | Glazed Classic | 6 | 2.50 | 15.00
Row 2: ITEM-002 | ORD-001 | Jelly Filled | 2 | 3.00 | 6.00
Row 3: ITEM-003 | ORD-002 | Chocolate Frosted | 4 | 2.75 | 11.00
Row 4: ITEM-004 | ORD-003 | Boston Cream | 4 | 3.00 | 12.00
Row 5: ITEM-005 | ORD-004 | Maple Bacon | 5 | 3.00 | 15.00
Row 6: ITEM-006 | ORD-005 | Chocolate Frosted | 12 | 2.75 | 33.00
Row 7: ITEM-007 | ORD-005 | Glazed Classic | 3 | 2.50 | 7.50
Row 8: ITEM-008 | ORD-005 | Jelly Filled | 0.5 | 3.00 | 1.00
Row 9: ITEM-009 | ORD-006 | Maple Bacon | 6 | 3.00 | 18.00
Row 10: ITEM-010 | ORD-006 | Boston Cream | 0.25 | 3.00 | 0.75
```

**Why This Schema?**
- ✅ Follows OLTP best practices (normalized)
- ✅ One order can have multiple products
- ✅ Tracks price at time of order (historical accuracy)
- ✅ Easy to calculate order totals
- ✅ Supports partial quantities (e.g., half dozen)

### Database 4: Suppliers 📦

Track ingredient suppliers and orders.

| Property | Type | Options/Notes |
|----------|------|---------------|
| Supplier Name | Title | Company name |
| Product Type | Select | Flour, Sugar, Chocolate, Filling, Other |
| Contact Email | Email | - |
| Phone | Phone | - |
| Last Order Date | Date | - |
| Next Order Date | Date | - |
| Reliable | Checkbox | Good track record? |
| Rating | Number | 1-5 stars |

**Sample Data:**
```
Row 1: Flour Mills Co | Flour | flour@example.com | 555-0101 | 2024-02-10 | 2024-02-20 | ✓ | 5
Row 2: Sweet Supply | Sugar | sweet@example.com | 555-0102 | 2024-02-12 | 2024-02-22 | ✓ | 4
Row 3: Choco Delights | Chocolate | choco@example.com | 555-0103 | 2024-02-14 | 2024-02-24 | ✓ | 5
Row 4: Jelly King | Filling | jelly@example.com | 555-0104 | 2024-02-11 | 2024-02-25 | ✗ | 3
```

---

## 🔗 Setup Option 2: Multi-Source Database (Advanced)

**What is a Multi-Source Database?**
Notion's new API (2025-09-03) allows connecting multiple data sources (like Google Sheets, CSV imports, or external databases) to a single Notion database. This lets you combine data from different sources in one view.

### Creating a Multi-Source Database:

1. **Create the main database: "Donut Shop Operations"**

2. **Add Data Source 1: Products** (Manual/Notion data)
   - Name, Category, Price, Cost, In Stock, Daily Limit, Calories
   - Add the same sample product data as above

3. **Add Data Source 2: Orders** (Could be CSV import or external source)
   - In Notion, click "..." → "Add data source" → Choose source type
   - Map columns: Order ID, Product, Quantity, Customer Name, etc.

4. **Add Data Source 3: Suppliers** (Could be Google Sheets sync)
   - Add another data source with supplier information
   - Map columns accordingly

### Multi-Source Testing:

```sql
-- The extension automatically detects and handles multiple data sources
-- You don't need to specify data_source_id - it's handled internally

SELECT * FROM read_notion('multi_source_database_id');

-- Extension will:
-- 1. Detect multiple data sources
-- 2. Query each source
-- 3. Combine results automatically
```

**Note:** Multi-source setup is more complex. For initial testing, use **Setup Option 1** (three separate databases).

---

## Step-by-Step Testing

### 1. Setup Authentication

```bash
# For Setup Option 1 (4 separate databases - proper OLTP schema)
echo "NOTION_TOKEN=secret_your_token_here" > .env
echo "NOTION_PRODUCTS_DB=your_products_database_id" >> .env
echo "NOTION_ORDERS_DB=your_orders_database_id" >> .env
echo "NOTION_ORDER_ITEMS_DB=your_order_items_database_id" >> .env
echo "NOTION_SUPPLIERS_DB=your_suppliers_database_id" >> .env

# For Setup Option 2 (multi-source)
echo "NOTION_TOKEN=secret_your_token_here" > .env
echo "NOTION_MULTISOURCE_DB=your_multisource_database_id" >> .env
```

### 2. Get Your Database IDs

For each database:
1. Open the database in Notion
2. Click "..." → "Copy link"
3. Extract the ID from URL: `https://notion.so/workspace/DATABASE_ID?v=view_id`

### 3. Share Databases with Integration

1. Go to https://www.notion.so/my-integrations
2. Create or select your integration
3. In each database: "..." → "Add connections" → Select your integration
4. **For multi-source:** Ensure integration has access to ALL data sources

### 4. Test Basic Read

**Option 1 (Separate Databases):**
```sql
LOAD notion;

-- View all products
SELECT * FROM read_notion('your_products_database_id');

-- View all orders (header)
SELECT * FROM read_notion('your_orders_database_id');

-- View all order items (line items)
SELECT * FROM read_notion('your_order_items_database_id');

-- View all suppliers
SELECT * FROM read_notion('your_suppliers_database_id');
```

**Option 2 (Multi-Source):**
```sql
LOAD notion;

-- Extension automatically handles multiple data sources
SELECT * FROM read_notion('your_multisource_database_id');

-- The extension will:
-- 1. Detect data sources
-- 2. Query each one
-- 3. Return combined results
```

### 5. Test Analytics Queries

```sql
-- Total revenue by product (proper join through order_items)
SELECT
    p.name as product,
    COUNT(DISTINCT oi.order_id) as num_orders,
    SUM(oi.quantity) as units_sold,
    SUM(oi.line_total) as revenue
FROM read_notion('products_db_id') p
LEFT JOIN read_notion('order_items_db_id') oi ON p.id = oi.product_id
LEFT JOIN read_notion('orders_db_id') o ON oi.order_id = o.id
WHERE o.completed = true
GROUP BY p.name
ORDER BY revenue DESC;

-- Profit margin by product
SELECT
    name,
    price,
    cost,
    ROUND(price - cost, 2) as profit_per_unit,
    ROUND(100.0 * (price - cost) / price, 1) as margin_pct
FROM read_notion('products_db_id')
WHERE in_stock = true
ORDER BY margin_pct DESC;

-- Daily sales summary
SELECT
    o.order_date,
    COUNT(DISTINCT o.order_id) as num_orders,
    SUM(oi.quantity) as total_donuts,
    SUM(o.total_amount) as daily_revenue
FROM read_notion('orders_db_id') o
LEFT JOIN read_notion('order_items_db_id') oi ON o.id = oi.order_id
WHERE o.completed = true
GROUP BY o.order_date
ORDER BY o.order_date DESC;

-- Top customers
SELECT
    customer_name,
    COUNT(*) as num_orders,
    SUM(total_amount) as total_spent
FROM read_notion('orders_db_id')
WHERE completed = true
GROUP BY customer_name
ORDER BY total_spent DESC
LIMIT 5;

-- Payment method breakdown
SELECT
    payment_method,
    COUNT(*) as transactions,
    SUM(total_amount) as total_amount
FROM read_notion('orders_db_id')
WHERE completed = true
GROUP BY payment_method;

-- Products running low on stock
SELECT
    name,
    category,
    daily_limit,
    in_stock
FROM read_notion('products_db_id')
WHERE daily_limit < 100
ORDER BY daily_limit;
```

### 6. Test Writing Data

```sql
-- Add new products from CSV
CREATE TABLE new_products (
    name VARCHAR,
    category VARCHAR,
    price DOUBLE,
    in_stock BOOLEAN
);

INSERT INTO new_products VALUES
    ('Strawberry Frosted', 'Classic', 2.75, true),
    ('Lemon Filled', 'Filled', 3.00, true),
    ('Birthday Cake', 'Specialty', 4.50, false);

-- Write to Notion (works for both single and multi-source)
COPY new_products TO 'your_products_database_id' (FORMAT notion);

-- Add new orders (header)
CREATE TABLE new_orders (
    order_id VARCHAR,
    customer_name VARCHAR,
    order_date DATE,
    completed BOOLEAN,
    total_amount DOUBLE,
    payment_method VARCHAR
);

INSERT INTO new_orders VALUES
    ('ORD-007', 'Sarah Johnson', '2024-02-17', false, 0, 'Credit Card'),
    ('ORD-008', 'Mike Peters', '2024-02-17', false, 0, 'Cash');

COPY new_orders TO 'your_orders_database_id' (FORMAT notion);

-- Add order line items
CREATE TABLE new_order_items (
    item_id VARCHAR,
    order_id VARCHAR,
    quantity INTEGER,
    unit_price DOUBLE,
    line_total DOUBLE
);

INSERT INTO new_order_items VALUES
    ('ITEM-011', 'ORD-007', 10, 2.50, 25.00),
    ('ITEM-012', 'ORD-007', 5, 3.00, 15.00),
    ('ITEM-013', 'ORD-008', 6, 2.75, 16.50);

COPY new_order_items TO 'your_order_items_database_id' (FORMAT notion);
```

## Business Analytics Scenarios

### Scenario 1: End of Day Report
```sql
SELECT
    'Daily Summary' as report_type,
    COUNT(DISTINCT order_id) as orders_today,
    SUM(quantity) as donuts_sold,
    SUM(total_amount) as revenue,
    ROUND(AVG(total_amount), 2) as avg_order_value
FROM read_notion('orders_db_id')
WHERE order_date = CURRENT_DATE
  AND completed = true;
```

### Scenario 2: Inventory Check
```sql
SELECT
    name,
    category,
    daily_limit,
    in_stock,
    CASE
        WHEN NOT in_stock THEN 'OUT OF STOCK'
        WHEN daily_limit < 50 THEN 'LOW INVENTORY'
        ELSE 'OK'
    END as status
FROM read_notion('products_db_id')
ORDER BY in_stock, daily_limit;
```

### Scenario 3: Customer Analysis
```sql
SELECT
    o.customer_name,
    COUNT(*) as visits,
    STRING_AGG(DISTINCT p.name, ', ') as favorite_products,
    SUM(o.total_amount) as lifetime_value
FROM read_notion('orders_db_id') o
JOIN read_notion('products_db_id') p ON o.product_id = p.id
WHERE o.completed = true
GROUP BY o.customer_name
HAVING COUNT(*) >= 2
ORDER BY lifetime_value DESC;
```

### Scenario 4: Supplier Performance
```sql
SELECT
    supplier_name,
    product_type,
    last_order_date,
    next_order_date,
    DATEDIFF('day', CURRENT_DATE, next_order_date::DATE) as days_until_order,
    CASE
        WHEN DATEDIFF('day', CURRENT_DATE, next_order_date::DATE) <= 3 THEN 'ORDER NOW'
        WHEN DATEDIFF('day', CURRENT_DATE, next_order_date::DATE) <= 7 THEN 'ORDER SOON'
        ELSE 'OK'
    END as urgency
FROM read_notion('suppliers_db_id')
WHERE reliable = true
ORDER BY next_order_date;
```

### Scenario 5: Product Performance
```sql
SELECT
    p.name,
    p.category,
    COUNT(o.order_id) as times_ordered,
    SUM(o.quantity) as total_sold,
    p.price - p.cost as profit_per_unit,
    (p.price - p.cost) * SUM(o.quantity) as total_profit
FROM read_notion('products_db_id') p
LEFT JOIN read_notion('orders_db_id') o ON p.id = o.product_id
WHERE o.completed = true
GROUP BY p.name, p.category, p.price, p.cost
ORDER BY total_profit DESC;
```

## API Version Considerations

### Single-Source vs Multi-Source

**Single-Source (Traditional):**
- ✅ Easier to set up
- ✅ More straightforward
- ✅ Each database = one data source
- ✅ Recommended for beginners

**Multi-Source (New in 2025-09-03):**
- ✅ Tests new API features
- ✅ Combines multiple data sources
- ✅ Extension handles `data_source_id` automatically
- ⚠️ More complex to set up
- ⚠️ Requires understanding of Notion's data source feature

The extension supports both! It automatically detects:
- Single-source: Uses traditional API calls
- Multi-source: Includes `data_source_id` in requests

## Expected Results

✅ **Success Indicators:**
- All databases are readable
- Joins between Orders ↔ Products work correctly
- Revenue calculations are accurate
- Writing new products/orders creates pages in Notion
- Analytics queries return meaningful business insights
- Multi-source databases work seamlessly (if testing)

⚠️ **Common Issues:**
- "Database not found" → Ensure database is shared with integration
- "Property mismatch" → Check property names match exactly (case-sensitive)
- "Relation not found" → Make sure Products database is linked in Orders
- "Data source error" → For multi-source, ensure all sources are accessible

## Performance Benchmarks

Expected query times:

- Read all products (6 rows): < 1 second
- Read all orders (6+ rows): 1-2 seconds
- Join orders with products: 2-3 seconds
- Write 3 new products: 3-5 seconds
- Multi-source query: 2-4 seconds (depending on # of sources)

## Next Steps

1. ✅ Choose Setup Option 1 (beginners) or Option 2 (advanced)
2. ✅ Set up your Notion databases
3. ✅ Add sample data (or use real donut shop data!)
4. ✅ Run the test queries
5. ✅ Try the business scenarios
6. 🎥 Record a demo showing the queries in action
7. 📊 Create visualizations of your results

## Bonus: Create a Daily Dashboard

```python
import duckdb
import plotly.express as px

con = duckdb.connect()
con.execute("LOAD notion")

# Daily revenue trend
df = con.execute("""
    SELECT order_date, SUM(total_amount) as revenue
    FROM read_notion('orders_db_id')
    WHERE completed = true
    GROUP BY order_date
    ORDER BY order_date
""").df()

fig = px.line(df, x='order_date', y='revenue', title='📈 Daily Revenue')
fig.show()
```

---

**Pro Tip:** Start with **Setup Option 1** (three separate databases) to learn the basics. Once comfortable, try **Setup Option 2** (multi-source) to test the advanced 2025-09-03 API features!
