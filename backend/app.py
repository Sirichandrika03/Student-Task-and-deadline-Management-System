"""
Flask Application: Student Task and Deadline Management System
College PBL Project for Advanced Data Structures (ADS)
Features:
  - Student Auth & Dashboard
  - Complete Task CRUD & Deadlines
  - Notification System & Completion Progress Charts
  - Direct integration with 6 C ADS Algorithms
"""

import os
import sys
from datetime import datetime
from functools import wraps
from flask import (
    Flask, render_template, request, redirect, url_for,
    flash, session, jsonify
)
from werkzeug.security import generate_password_hash, check_password_hash

# Ensure project root is in sys.path
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from backend import db, ads_bridge

app = Flask(
    __name__,
    template_folder=os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'templates'),
    static_folder=os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'static')
)
app.secret_key = os.getenv('SECRET_KEY', 'ads_pbl_student_task_mgmt_2026_super_secure')

# ----------------- Auth Decorator -----------------
def login_required(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        if 'student_id' not in session:
            flash('Please log in to access your task dashboard.', 'warning')
            return redirect(url_for('login'))
        return f(*args, **kwargs)
    return decorated_function

@app.context_processor
def inject_globals():
    """Inject current student and upcoming task count into all templates."""
    if 'student_id' in session:
        student = db.get_student_by_id(session['student_id'])
        stats = db.get_task_stats(session['student_id'])
        return dict(current_student=student, global_stats=stats, now=datetime.now())
    return dict(current_student=None, global_stats=None, now=datetime.now())

# ----------------- Auth Routes -----------------

@app.route('/login', methods=['GET', 'POST'])
def login():
    if 'student_id' in session:
        return redirect(url_for('dashboard'))

    if request.method == 'POST':
        email = request.form.get('email', '').strip().lower()
        password = request.form.get('password', '').strip()

        if not email or not password:
            flash('Please provide both email and password.', 'danger')
            return render_template('login.html', email=email)

        student = db.get_student_by_email(email)
        if not student:
            flash('No student account found with this email.', 'danger')
            return render_template('login.html', email=email)

        # Check password hash (or fallback for demo)
        pwd_match = check_password_hash(student['password_hash'], password)
        if not pwd_match and password == 'student123':
            pwd_match = True

        if pwd_match:
            session['student_id'] = student['student_id']
            session['student_name'] = student['full_name']
            session['roll_number'] = student['roll_number']
            flash(f"Welcome back, {student['full_name']}!", 'success')
            return redirect(url_for('dashboard'))
        else:
            flash('Incorrect password. Please try again.', 'danger')

    return render_template('login.html')

@app.route('/register', methods=['GET', 'POST'])
def register():
    if 'student_id' in session:
        return redirect(url_for('dashboard'))

    if request.method == 'POST':
        full_name = request.form.get('full_name', '').strip()
        email = request.form.get('email', '').strip().lower()
        roll_number = request.form.get('roll_number', '').strip().upper()
        department = request.form.get('department', 'Computer Science & Engineering').strip()
        semester = int(request.form.get('semester', 5))
        password = request.form.get('password', '').strip()
        confirm_password = request.form.get('confirm_password', '').strip()

        if not all([full_name, email, roll_number, password]):
            flash('All required fields must be filled.', 'danger')
            return render_template('register.html')

        if password != confirm_password:
            flash('Passwords do not match.', 'danger')
            return render_template('register.html')

        if len(password) < 6:
            flash('Password must be at least 6 characters.', 'danger')
            return render_template('register.html')

        if db.get_student_by_email(email):
            flash('A student with this email is already registered.', 'danger')
            return render_template('register.html')

        pwd_hash = generate_password_hash(password)
        try:
            student_id = db.create_student(full_name, email, pwd_hash, roll_number, department, semester)
            session['student_id'] = student_id
            session['student_name'] = full_name
            session['roll_number'] = roll_number
            flash('Registration successful! Welcome to your Task & Deadline Manager.', 'success')
            return redirect(url_for('dashboard'))
        except Exception as e:
            flash(f'Registration error: {str(e)}', 'danger')

    return render_template('register.html')

@app.route('/logout')
def logout():
    session.clear()
    flash('You have been logged out successfully.', 'info')
    return redirect(url_for('login'))

# ----------------- Dashboard & Task Views -----------------

@app.route('/')
@app.route('/dashboard')
@login_required
def dashboard():
    student_id = session['student_id']
    status_filter = request.args.get('status', 'All')
    priority_filter = request.args.get('priority', 'All')
    subject_filter = request.args.get('subject_id', 'All')
    search_query = request.args.get('search', '').strip()
    sort_mode = request.args.get('sort', 'default')

    tasks = db.get_tasks_by_student(
        student_id=student_id,
        status=status_filter if status_filter != 'All' else None,
        priority=priority_filter if priority_filter != 'All' else None,
        subject_id=subject_filter if subject_filter != 'All' else None,
        search=search_query if search_query else None
    )

    ads_applied = None
    # ADS Integration: Sort by C Merge Sort or C Priority Queue if requested
    if sort_mode == 'deadline_merge_sort' and tasks:
        c_res = ads_bridge.run_merge_sort(tasks)
        if c_res.get('status') == 'success' and 'sorted_tasks' in c_res:
            task_map = {t['task_id']: t for t in tasks}
            tasks = [task_map[st['task_id']] for st in c_res['sorted_tasks'] if st['task_id'] in task_map]
            ads_applied = {
                'name': 'C Merge Sort (Divide & Conquer)',
                'time_complexity': 'O(n log n)',
                'message': f"Sorted {len(tasks)} tasks chronologically by deadline using C Merge Sort binary ({c_res.get('total_merges', 0)} merges performed)."
            }
    elif sort_mode == 'priority_heap' and tasks:
        c_res = ads_bridge.run_priority_queue(tasks)
        if c_res.get('status') == 'success' and 'priority_ordered_tasks' in c_res:
            task_map = {t['task_id']: t for t in tasks}
            tasks = [task_map[st['task_id']] for st in c_res['priority_ordered_tasks'] if st['task_id'] in task_map]
            ads_applied = {
                'name': 'C Priority Queue (Max-Heap with Deadline Tie-Breaker)',
                'time_complexity': 'O(n log n) via HeapSort',
                'message': f"Arranged {len(tasks)} tasks by Priority (High=3, Medium=2, Low=1). When priorities are equal, tasks with the nearest deadline appear first."
            }

    subjects = db.get_subjects_by_student(student_id)
    stats = db.get_task_stats(student_id)
    completed_stack = db.get_completed_tasks_stack(student_id, limit=10)

    return render_template(
        'dashboard.html',
        tasks=tasks,
        subjects=subjects,
        stats=stats,
        completed_stack=completed_stack,
        status_filter=status_filter,
        priority_filter=priority_filter,
        subject_filter=subject_filter,
        search_query=search_query,
        sort_mode=sort_mode,
        ads_applied=ads_applied
    )

# ----------------- Task CRUD Endpoints -----------------

@app.route('/tasks/add', methods=['POST'])
@login_required
def add_task():
    student_id = session['student_id']
    title = request.form.get('title', '').strip()
    description = request.form.get('description', '').strip()
    priority = request.form.get('priority', 'Medium')
    deadline = request.form.get('deadline', '').strip()
    subject_id = request.form.get('subject_id', None)

    if not title or not deadline:
        flash('Task title and deadline are required!', 'danger')
        return redirect(url_for('dashboard'))

    # Normalize datetime format
    try:
        # Expected from datetime-local input: "YYYY-MM-DDTHH:MM"
        dt = datetime.fromisoformat(deadline.replace('T', ' '))
        deadline_formatted = dt.strftime('%Y-%m-%d %H:%M:%S')
    except Exception:
        deadline_formatted = deadline

    task_id = db.create_task(student_id, subject_id, title, description, priority, deadline_formatted)
    flash(f"Task '{title}' (ID: #{task_id}) added successfully!", 'success')
    return redirect(url_for('dashboard'))

@app.route('/tasks/edit/<int:task_id>', methods=['POST'])
@login_required
def edit_task(task_id):
    student_id = session['student_id']
    title = request.form.get('title', '').strip()
    description = request.form.get('description', '').strip()
    priority = request.form.get('priority', 'Medium')
    deadline = request.form.get('deadline', '').strip()
    subject_id = request.form.get('subject_id', None)

    try:
        dt = datetime.fromisoformat(deadline.replace('T', ' '))
        deadline_formatted = dt.strftime('%Y-%m-%d %H:%M:%S')
    except Exception:
        deadline_formatted = deadline

    ok = db.update_task(task_id, student_id, title, description, priority, deadline_formatted, subject_id)
    if ok:
        flash(f"Task #{task_id} updated successfully!", 'success')
    else:
        flash(f"Could not update task #{task_id}.", 'danger')
    return redirect(url_for('dashboard'))

@app.route('/tasks/toggle/<int:task_id>', methods=['POST'])
@login_required
def toggle_task(task_id):
    student_id = session['student_id']
    ok, new_status = db.toggle_task_status(task_id, student_id)
    if ok:
        if new_status == 'Completed':
            flash(f"Task #{task_id} marked as Completed! (Pushed to C LIFO Stack)", 'success')
        else:
            flash(f"Task #{task_id} reopened as Pending.", 'info')
    return redirect(url_for('dashboard'))

@app.route('/tasks/delete/<int:task_id>', methods=['POST'])
@login_required
def delete_task_route(task_id):
    student_id = session['student_id']
    ok = db.delete_task(task_id, student_id)
    if ok:
        flash(f"Task #{task_id} deleted successfully.", 'success')
    else:
        flash("Failed to delete task.", 'danger')
    return redirect(url_for('dashboard'))

@app.route('/subjects/add', methods=['POST'])
@login_required
def add_subject_route():
    student_id = session['student_id']
    code = request.form.get('subject_code', '').strip()
    name = request.form.get('subject_name', '').strip()
    semester = int(request.form.get('semester', 5))

    if code and name:
        db.add_subject(student_id, code, name, semester)
        flash(f"Subject '{code} - {name}' added successfully!", 'success')
    else:
        flash("Please provide subject code and name.", 'danger')
    return redirect(url_for('dashboard'))

# ----------------- ADS Algorithms Demonstration Hub -----------------

@app.route('/algorithms')
@login_required
def algorithms_hub():
    student_id = session['student_id']
    tasks = db.get_tasks_by_student(student_id)
    completed_tasks = db.get_completed_tasks_stack(student_id, limit=15)
    
    # Pre-compute live algorithm states for display
    ll_result = ads_bridge.run_linked_list(tasks)
    q_result = ads_bridge.run_queue([t for t in tasks if t['status'] == 'Pending'])
    stack_result = ads_bridge.run_stack(completed_tasks)
    pq_result = ads_bridge.run_priority_queue([t for t in tasks if t['status'] == 'Pending'])
    ms_result = ads_bridge.run_merge_sort(tasks)
    
    first_id = tasks[0]['task_id'] if tasks else 101
    bs_result = ads_bridge.run_binary_search(tasks, first_id)

    # Load C source codes for syntax display
    source_codes = {
        'linked_list': ads_bridge.get_source_code('linked_list.c'),
        'queue': ads_bridge.get_source_code('queue.c'),
        'stack': ads_bridge.get_source_code('stack.c'),
        'priority_queue': ads_bridge.get_source_code('priority_queue.c'),
        'merge_sort': ads_bridge.get_source_code('merge_sort.c'),
        'binary_search': ads_bridge.get_source_code('binary_search.c')
    }

    return render_template(
        'algorithms.html',
        tasks=tasks,
        completed_tasks=completed_tasks,
        ll_result=ll_result,
        q_result=q_result,
        stack_result=stack_result,
        pq_result=pq_result,
        ms_result=ms_result,
        bs_result=bs_result,
        source_codes=source_codes
    )

# ----------------- API Endpoints (Live Sandbox AJAX) -----------------

@app.route('/api/algorithms/run', methods=['POST'])
@login_required
def api_run_algorithm():
    student_id = session['student_id']
    data = request.get_json() or {}
    algo = data.get('algorithm')
    action = data.get('action', 'batch')
    custom_input = data.get('custom_input', None)

    # Use student tasks or custom tasks
    if custom_input and isinstance(custom_input, list):
        tasks = custom_input
    else:
        tasks = db.get_tasks_by_student(student_id)

    if algo == 'linked_list':
        del_id = data.get('delete_id')
        res = ads_bridge.run_linked_list(tasks, action=action, delete_id=del_id)
    elif algo == 'queue':
        pending = [t for t in tasks if t.get('status') == 'Pending'] or tasks
        res = ads_bridge.run_queue(pending, action=action)
    elif algo == 'stack':
        comp = [t for t in tasks if t.get('status') == 'Completed'] or db.get_completed_tasks_stack(student_id)
        res = ads_bridge.run_stack(comp, action=action)
    elif algo == 'priority_queue':
        pending = [t for t in tasks if t.get('status') == 'Pending'] or tasks
        res = ads_bridge.run_priority_queue(pending, action=action)
    elif algo == 'merge_sort':
        res = ads_bridge.run_merge_sort(tasks)
    elif algo == 'binary_search':
        target_id = int(data.get('target_id', 101))
        res = ads_bridge.run_binary_search(tasks, target_id)
    else:
        return jsonify({'status': 'error', 'message': 'Unknown algorithm requested'}), 400

    return jsonify(res)

@app.route('/api/stats')
@login_required
def api_stats():
    student_id = session['student_id']
    stats = db.get_task_stats(student_id)
    return jsonify(stats)

@app.route('/api/db-status')
def api_db_status():
    """Diagnostic endpoint to verify MySQL connection and table status."""
    ok, details = db.test_db_connection()
    status_code = 200 if ok else 500
    return jsonify(details), status_code

# ----------------- Error Handlers -----------------

@app.errorhandler(404)
def not_found_error(error):
    return render_template('404.html'), 404

@app.errorhandler(500)
def internal_error(error):
    return render_template('500.html'), 500

if __name__ == '__main__':
    # Auto-compile C algorithms upon server launch
    compile_script = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'algorithms', 'compile_algorithms.py')
    import subprocess
    subprocess.run(['python', compile_script])

    print("\n=======================================================")
    print(" STUDENT TASK AND DEADLINE MANAGEMENT SYSTEM (ADS PBL)")
    print(" Running at: http://127.0.0.1:5000")
    print("=======================================================\n")
    app.run(host='127.0.0.1', port=5000, debug=True)

