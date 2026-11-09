"""
Main Entry Point: Student Task and Deadline Management System (ADS PBL)
Executes C algorithm compilation and runs the Flask application server.
"""

import os
import sys
import subprocess

# Determine directories
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
ALGO_DIR = os.path.join(BASE_DIR, 'algorithms')
COMPILE_SCRIPT = os.path.join(ALGO_DIR, 'compile_algorithms.py')

def main():
    print("==================================================================")
    print("    STUDENT TASK AND DEADLINE MANAGEMENT SYSTEM - ADS PBL         ")
    print("==================================================================")
    print("[1/3] Compiling Native C Algorithms...")
    res = subprocess.run([sys.executable, COMPILE_SCRIPT])
    if res.returncode != 0:
        print("Warning: Compilation script returned non-zero code. Check compilers.")
    else:
        print("[SUCCESS] All 6 C binaries compiled in algorithms/bin/")

    print("\n[2/3] Checking Database Connection & Seeding...")
    try:
        from backend import db
        test_student = db.get_student_by_id(1)
        if test_student:
            print(f"[SUCCESS] Connected to MySQL: student_task_db (Found demo student '{test_student['full_name']}')")
        else:
            print("[INFO] Initializing student_task_db...")
            from database import init_db
            init_db.init_database()
    except Exception as e:
        print(f"[WARNING] Database check encountered: {e}")
        print("Attempting to run database/init_db.py...")
        init_script = os.path.join(BASE_DIR, 'database', 'init_db.py')
        subprocess.run([sys.executable, init_script])

    print("\n[3/3] Starting Flask Application Server...")
    print(">> Open your web browser at: http://127.0.0.1:5000")
    print(">> Demo Login: student@college.edu | Password: student123")
    print("==================================================================\n")

    from backend.app import app
    app.run(host='127.0.0.1', port=5000, debug=True)

if __name__ == '__main__':
    main()

