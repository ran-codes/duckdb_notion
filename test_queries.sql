-- DuckDB Notion Extension - Test Queries
-- Replace database_id placeholders with your actual Notion database IDs

-- 1. VIEW ALL PROJECTS
SELECT * FROM read_notion('projects_database_id');

-- 2. COUNT TASKS BY STATUS
SELECT
    status,
    COUNT(*) as count
FROM read_notion('tasks_database_id')
GROUP BY status
ORDER BY count DESC;

-- 3. PROJECT PROGRESS OVERVIEW
SELECT
    name as project,
    status,
    progress,
    CASE
        WHEN progress >= 80 THEN '🟢 Nearly Done'
        WHEN progress >= 50 THEN '🟡 Half Way'
        WHEN progress >= 20 THEN '🟠 Started'
        ELSE '🔴 Just Beginning'
    END as progress_status
FROM read_notion('projects_database_id')
WHERE active = true
ORDER BY progress DESC;

-- 4. HIGH PRIORITY INCOMPLETE TASKS
SELECT
    task_name,
    status,
    priority,
    due_date
FROM read_notion('tasks_database_id')
WHERE completed = false
  AND priority <= 2
ORDER BY priority, due_date;

-- 5. JOIN: TASKS PER PROJECT
SELECT
    p.name as project,
    COUNT(t.id) as total_tasks,
    SUM(CASE WHEN t.completed THEN 1 ELSE 0 END) as completed_tasks,
    ROUND(100.0 * SUM(CASE WHEN t.completed THEN 1 ELSE 0 END) / COUNT(t.id), 1) as completion_pct
FROM read_notion('projects_database_id') p
LEFT JOIN read_notion('tasks_database_id') t
    ON p.id = t.project_id
GROUP BY p.name
ORDER BY completion_pct DESC;

-- 6. RESOURCES BY TYPE
SELECT
    type,
    COUNT(*) as count,
    AVG(rating) as avg_rating
FROM read_notion('notes_database_id')
GROUP BY type
ORDER BY count DESC;

-- 7. OVERDUE TASKS
SELECT
    task_name,
    due_date,
    status,
    assignee
FROM read_notion('tasks_database_id')
WHERE completed = false
  AND due_date < CURRENT_DATE
ORDER BY due_date;

-- 8. PROJECT TIMELINE
SELECT
    name as project,
    start_date,
    target_date,
    DATEDIFF('day', start_date::DATE, target_date::DATE) as duration_days,
    status
FROM read_notion('projects_database_id')
ORDER BY start_date;

-- 9. PRODUCTIVITY METRICS
SELECT
    assignee,
    COUNT(*) as total_tasks,
    SUM(CASE WHEN completed THEN 1 ELSE 0 END) as completed,
    SUM(estimated_hours) as estimated_hours,
    ROUND(AVG(CASE WHEN completed THEN estimated_hours END), 1) as avg_task_hours
FROM read_notion('tasks_database_id')
GROUP BY assignee;

-- 10. WRITE: ADD NEW TASKS FROM CSV
-- First create a CSV file: new_tasks.csv
-- task_name,priority,status,completed
-- "Review documentation",1,"To Do",false
-- "Update tests",2,"To Do",false

-- Then copy to Notion:
COPY (SELECT * FROM read_csv('new_tasks.csv'))
TO 'tasks_database_id' (FORMAT notion);

-- 11. COMPLEX: TOP RESOURCES PER PROJECT
SELECT
    p.name as project,
    n.title as resource,
    n.type,
    n.url
FROM read_notion('projects_database_id') p
LEFT JOIN read_notion('notes_database_id') n
    ON p.id = n.related_project_id
WHERE n.rating >= 4
ORDER BY p.name, n.rating DESC;

-- 12. WEEKLY PROGRESS REPORT
SELECT
    'Week ' || CAST(WEEK(created_time::DATE) AS VARCHAR) as week,
    COUNT(*) as tasks_created,
    SUM(CASE WHEN completed THEN 1 ELSE 0 END) as tasks_completed
FROM read_notion('tasks_database_id')
WHERE created_time::DATE >= CURRENT_DATE - INTERVAL 30 DAY
GROUP BY WEEK(created_time::DATE)
ORDER BY week;
