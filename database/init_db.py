import os
import mysql.connector
from werkzeug.security import generate_password_hash

DB_HOST = os.getenv('DB_HOST', 'localhost')
DB_USER = os.getenv('DB_USER', 'root')
DB_PASS = os.getenv('DB_PASS', 'Siri-25r11a62e6')
DB_PORT = int(os.getenv('DB_PORT', 3306))
DB_NAME = os.getenv('DB_NAME', 'student_task_db')

def init_database():
    print(f"Connecting to MySQL server at {DB_HOST}:{DB_PORT} as {DB_USER} using mysql-connector-python...")
    conn = mysql.connector.connect(
        host=DB_HOST,
        user=DB_USER,
        password=DB_PASS,
        port=DB_PORT,
        autocommit=True
    )
    cursor = conn.cursor()
    
    # Read schema.sql
    schema_path = os.path.join(os.path.dirname(__file__), 'schema.sql')
    with open(schema_path, 'r', encoding='utf-8') as f:
        sql_content = f.read()

    # Split commands by semicolon, handling statements cleanly
    commands = [cmd.strip() for cmd in sql_content.split(';') if cmd.strip()]
    
    for cmd in commands:
        if cmd:
            cursor.execute(cmd)
    
    print("Database `student_task_db` and tables created successfully!")
    
    # Ensure students have correct hashed passwords for 'student123'
    cursor.execute("USE student_task_db")
    hashed = generate_password_hash('student123')
    cursor.execute("UPDATE Students SET password_hash = %s WHERE student_id IN (1, 2)", (hashed,))
    
    cursor.execute("SELECT student_id, full_name, email, roll_number FROM Students")
    students = cursor.fetchall()
    print("Seeded Students:")
    for s in students:
        print(f"  ID: {s[0]} | Name: {s[1]} | Email: {s[2]} | Roll: {s[3]}")

    cursor.execute("SELECT COUNT(*) FROM Tasks")
    t_count = cursor.fetchone()[0]
    print(f"Seeded Tasks count: {t_count}")

    cursor.close()
    conn.close()
    print("Database initialization complete!")

if __name__ == '__main__':
    init_database()
