"""
Database Connection and CRUD Operations
Handles MySQL connectivity using `mysql-connector-python` for `student_task_db`.

Configuration:
  - Database: student_task_db
  - Host: localhost
  - User: root
  - Port: 3306
  - Password: Existing MySQL root password
"""

import os
import logging
from datetime import datetime
import mysql.connector
from mysql.connector import Error as MySQLError

logger = logging.getLogger(__name__)

# Connection parameters
DB_HOST = os.getenv('DB_HOST', 'localhost')
DB_USER = os.getenv('DB_USER', 'root')
DB_PASS = os.getenv('DB_PASS', 'Siri-25r11a62e6')
DB_NAME = os.getenv('DB_NAME', 'student_task_db')
DB_PORT = int(os.getenv('DB_PORT', 3306))

def get_connection():
    """
    Establish a connection to the MySQL database using mysql-connector-python.
    Includes proper error handling and autocommit enabled.
    """
    try:
        conn = mysql.connector.connect(
            host=DB_HOST,
            user=DB_USER,
            password=DB_PASS,
            database=DB_NAME,
            port=DB_PORT,
            autocommit=True
        )
        return conn
    except MySQLError as err:
        logger.error(f"MySQL Connection Error [{err.errno}]: {err.msg}")
        raise ConnectionError(
            f"Failed to connect to MySQL database '{DB_NAME}' at {DB_HOST}:{DB_PORT} as user '{DB_USER}'. "
            f"MySQL Error: {err.msg} (Code: {err.errno})"
        ) from err

def test_db_connection():
    """
    Diagnostic helper to test database connectivity, check tables and record counts.
    Returns (success: bool, info: dict)
    """
    try:
        conn = get_connection()
        cur = conn.cursor(dictionary=True)
        cur.execute("SELECT DATABASE() AS current_db, VERSION() AS version")
        db_info = cur.fetchone()
        
        # Check tables
        cur.execute("""
            SELECT table_name, table_rows 
            FROM information_schema.tables 
            WHERE table_schema = %s
        """, (DB_NAME,))
        tables = cur.fetchall()
        
        cur.close()
        conn.close()
        return True, {
            "status": "connected",
            "host": DB_HOST,
            "port": DB_PORT,
            "database": db_info.get("current_db"),
            "mysql_version": db_info.get("version"),
            "tables": tables
        }
    except Exception as e:
        logger.error(f"test_db_connection failed: {e}")
        return False, {"status": "error", "message": str(e)}

# ----------------- Student Auth Helpers -----------------

def get_student_by_email(email):
    conn = get_connection()
    try:
        cur = conn.cursor(dictionary=True)
        cur.execute("SELECT * FROM Students WHERE email = %s", (email.strip().lower(),))
        row = cur.fetchone()
        cur.close()
        return row
    except MySQLError as err:
        logger.error(f"Error querying student by email: {err}")
        raise
    finally:
        conn.close()

def get_student_by_id(student_id):
    conn = get_connection()
    try:
        cur = conn.cursor(dictionary=True)
        cur.execute(
            "SELECT student_id, full_name, email, roll_number, department, semester, created_at "
            "FROM Students WHERE student_id = %s", 
            (student_id,)
        )
        row = cur.fetchone()
        cur.close()
        return row
    except MySQLError as err:
        logger.error(f"Error querying student by ID: {err}")
        raise
    finally:
        conn.close()

def create_student(full_name, email, password_hash, roll_number, department="Computer Science & Engineering", semester=5):
    conn = get_connection()
    try:
        cur = conn.cursor(dictionary=True)
        cur.execute("""
            INSERT INTO Students (full_name, email, password_hash, roll_number, department, semester)
            VALUES (%s, %s, %s, %s, %s, %s)
        """, (full_name.strip(), email.strip().lower(), password_hash, roll_number.strip().upper(), department.strip(), semester))
        new_id = cur.lastrowid
        
        # Automatically create default starter subjects for the new student
        starter_subjects = [
            ("CS501", "Advanced Data Structures & Algorithms"),
            ("CS502", "Database Management Systems"),
            ("CS503", "Operating Systems"),
            ("CS504", "Computer Networks")
        ]
        for code, name in starter_subjects:
            cur.execute("""
                INSERT INTO Subjects (student_id, subject_code, subject_name, semester)
                VALUES (%s, %s, %s, %s)
            """, (new_id, code, name, semester))
            
        cur.close()
        return new_id
    except MySQLError as err:
        logger.error(f"Error creating student: {err}")
        raise
    finally:
        conn.close()

# ----------------- Subject Helpers -----------------

def get_subjects_by_student(student_id):
    conn = get_connection()
    try:
        cur = conn.cursor(dictionary=True)
        cur.execute("SELECT * FROM Subjects WHERE student_id = %s ORDER BY subject_code ASC", (student_id,))
        rows = cur.fetchall()
        cur.close()
        return rows
    except MySQLError as err:
        logger.error(f"Error getting subjects: {err}")
        raise
    finally:
        conn.close()

def add_subject(student_id, code, name, semester=5):
    conn = get_connection()
    try:
        cur = conn.cursor(dictionary=True)
        cur.execute("""
            INSERT INTO Subjects (student_id, subject_code, subject_name, semester)
            VALUES (%s, %s, %s, %s)
        """, (student_id, code.strip().upper(), name.strip(), semester))
        new_id = cur.lastrowid
        cur.close()
        return new_id
    except MySQLError as err:
        logger.error(f"Error adding subject: {err}")
        raise
    finally:
        conn.close()

# ----------------- Task Helpers -----------------

def get_tasks_by_student(student_id, status=None, subject_id=None, search=None, priority=None):
    conn = get_connection()
    try:
        cur = conn.cursor(dictionary=True)
        sql = """
            SELECT t.*, s.subject_code, s.subject_name
            FROM Tasks t
            LEFT JOIN Subjects s ON t.subject_id = s.subject_id
            WHERE t.student_id = %s
        """
        params = [student_id]

        if status and status != 'All':
            sql += " AND t.status = %s"
            params.append(status)

        if subject_id and str(subject_id).isdigit():
            sql += " AND t.subject_id = %s"
            params.append(int(subject_id))

        if priority and priority != 'All':
            sql += " AND t.priority = %s"
            params.append(priority)

        if search:
            sql += " AND (t.title LIKE %s OR t.description LIKE %s OR s.subject_name LIKE %s)"
            pattern = f"%{search.strip()}%"
            params.extend([pattern, pattern, pattern])

        sql += " ORDER BY t.deadline ASC, t.task_id ASC"
        cur.execute(sql, tuple(params))
        rows = cur.fetchall()
        cur.close()
        
        # Format dates nicely as strings for JSON/frontend
        for r in rows:
            if isinstance(r.get('deadline'), datetime):
                r['deadline_str'] = r['deadline'].strftime('%Y-%m-%d %H:%M')
                r['deadline_date'] = r['deadline'].strftime('%Y-%m-%d')
                r['deadline_iso'] = r['deadline'].isoformat()
            if isinstance(r.get('completed_at'), datetime):
                r['completed_at_str'] = r['completed_at'].strftime('%Y-%m-%d %H:%M')
            if isinstance(r.get('created_at'), datetime):
                r['created_at_str'] = r['created_at'].strftime('%Y-%m-%d %H:%M')
        return rows
    except MySQLError as err:
        logger.error(f"Error fetching tasks: {err}")
        raise
    finally:
        conn.close()

def get_task_by_id(task_id, student_id=None):
    conn = get_connection()
    try:
        cur = conn.cursor(dictionary=True)
        sql = """
            SELECT t.*, s.subject_code, s.subject_name
            FROM Tasks t
            LEFT JOIN Subjects s ON t.subject_id = s.subject_id
            WHERE t.task_id = %s
        """
        params = [task_id]
        if student_id:
            sql += " AND t.student_id = %s"
            params.append(student_id)
            
        cur.execute(sql, tuple(params))
        row = cur.fetchone()
        cur.close()
        
        if row:
            if isinstance(row.get('deadline'), datetime):
                row['deadline_str'] = row['deadline'].strftime('%Y-%m-%d %H:%M')
                row['deadline_date'] = row['deadline'].strftime('%Y-%m-%d')
                row['deadline_iso'] = row['deadline'].isoformat()
        return row
    except MySQLError as err:
        logger.error(f"Error getting task by ID: {err}")
        raise
    finally:
        conn.close()

def create_task(student_id, subject_id, title, description, priority, deadline):
    conn = get_connection()
    try:
        cur = conn.cursor(dictionary=True)
        cur.execute("""
            INSERT INTO Tasks (student_id, subject_id, title, description, priority, deadline, status)
            VALUES (%s, %s, %s, %s, %s, %s, 'Pending')
        """, (student_id, subject_id if subject_id else None, title.strip(), description.strip(), priority, deadline))
        new_id = cur.lastrowid
        cur.close()
        return new_id
    except MySQLError as err:
        logger.error(f"Error creating task: {err}")
        raise
    finally:
        conn.close()

def update_task(task_id, student_id, title, description, priority, deadline, subject_id=None):
    conn = get_connection()
    try:
        cur = conn.cursor(dictionary=True)
        cur.execute("""
            UPDATE Tasks 
            SET title = %s, description = %s, priority = %s, deadline = %s, subject_id = %s
            WHERE task_id = %s AND student_id = %s
        """, (title.strip(), description.strip(), priority, deadline, subject_id if subject_id else None, task_id, student_id))
        count = cur.rowcount
        cur.close()
        return count > 0
    except MySQLError as err:
        logger.error(f"Error updating task: {err}")
        raise
    finally:
        conn.close()

def toggle_task_status(task_id, student_id, target_status=None):
    conn = get_connection()
    try:
        cur = conn.cursor(dictionary=True)
        cur.execute("SELECT status FROM Tasks WHERE task_id = %s AND student_id = %s", (task_id, student_id))
        row = cur.fetchone()
        if not row:
            cur.close()
            return False, None
        
        curr = row['status']
        new_status = target_status if target_status else ('Completed' if curr == 'Pending' else 'Pending')
        comp_time = datetime.now() if new_status == 'Completed' else None
        
        cur.execute("""
            UPDATE Tasks 
            SET status = %s, completed_at = %s
            WHERE task_id = %s AND student_id = %s
        """, (new_status, comp_time, task_id, student_id))
        cur.close()
        return True, new_status
    except MySQLError as err:
        logger.error(f"Error toggling task status: {err}")
        raise
    finally:
        conn.close()

def delete_task(task_id, student_id):
    conn = get_connection()
    try:
        cur = conn.cursor(dictionary=True)
        cur.execute("DELETE FROM Tasks WHERE task_id = %s AND student_id = %s", (task_id, student_id))
        count = cur.rowcount
        cur.close()
        return count > 0
    except MySQLError as err:
        logger.error(f"Error deleting task: {err}")
        raise
    finally:
        conn.close()

def get_completed_tasks_stack(student_id, limit=30):
    conn = get_connection()
    try:
        cur = conn.cursor(dictionary=True)
        cur.execute("""
            SELECT t.task_id, t.title, t.priority, t.completed_at, t.updated_at, s.subject_code
            FROM Tasks t
            LEFT JOIN Subjects s ON t.subject_id = s.subject_id
            WHERE t.student_id = %s AND t.status = 'Completed'
            ORDER BY t.completed_at DESC, t.updated_at DESC
            LIMIT %s
        """, (student_id, limit))
        rows = cur.fetchall()
        cur.close()
        for r in rows:
            if isinstance(r.get('completed_at'), datetime):
                r['completed_at_str'] = r['completed_at'].strftime('%Y-%m-%d %H:%M')
        return rows
    except MySQLError as err:
        logger.error(f"Error fetching completed stack: {err}")
        raise
    finally:
        conn.close()

def get_task_stats(student_id):
    conn = get_connection()
    try:
        cur = conn.cursor(dictionary=True)
        cur.execute("""
            SELECT 
                COUNT(*) as total,
                SUM(CASE WHEN status = 'Completed' THEN 1 ELSE 0 END) as completed,
                SUM(CASE WHEN status = 'Pending' THEN 1 ELSE 0 END) as pending,
                SUM(CASE WHEN priority = 'High' AND status = 'Pending' THEN 1 ELSE 0 END) as high_pending,
                SUM(CASE WHEN priority = 'Medium' AND status = 'Pending' THEN 1 ELSE 0 END) as med_pending,
                SUM(CASE WHEN priority = 'Low' AND status = 'Pending' THEN 1 ELSE 0 END) as low_pending,
                SUM(CASE WHEN status = 'Pending' AND deadline <= DATE_ADD(NOW(), INTERVAL 48 HOUR) AND deadline >= NOW() THEN 1 ELSE 0 END) as urgent_48h,
                SUM(CASE WHEN status = 'Pending' AND deadline < NOW() THEN 1 ELSE 0 END) as overdue
            FROM Tasks
            WHERE student_id = %s
        """, (student_id,))
        res = cur.fetchone()
        
        total = (res.get('total') or 0) if res else 0
        completed = int(res.get('completed') or 0) if res else 0
        pending = int(res.get('pending') or 0) if res else 0
        rate = round((completed / total * 100), 1) if total > 0 else 0.0
        
        # Fetch upcoming pending tasks for notification banner
        cur.execute("""
            SELECT task_id, title, priority, deadline
            FROM Tasks
            WHERE student_id = %s AND status = 'Pending' AND deadline >= NOW()
            ORDER BY deadline ASC
            LIMIT 5
        """, (student_id,))
        upcoming = cur.fetchall()
        cur.close()
        
        for u in upcoming:
            if isinstance(u.get('deadline'), datetime):
                u['deadline_str'] = u['deadline'].strftime('%Y-%m-%d %H:%M')
                u['days_left'] = round((u['deadline'] - datetime.now()).total_seconds() / 86400, 1)

        return {
            'total': total,
            'completed': completed,
            'pending': pending,
            'completion_rate': rate,
            'high_pending': int(res.get('high_pending') or 0) if res else 0,
            'med_pending': int(res.get('med_pending') or 0) if res else 0,
            'low_pending': int(res.get('low_pending') or 0) if res else 0,
            'urgent_48h': int(res.get('urgent_48h') or 0) if res else 0,
            'overdue': int(res.get('overdue') or 0) if res else 0,
            'upcoming_tasks': upcoming
        }
    except MySQLError as err:
        logger.error(f"Error computing task stats: {err}")
        raise
    finally:
        conn.close()
