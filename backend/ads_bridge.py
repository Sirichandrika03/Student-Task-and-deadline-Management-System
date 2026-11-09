"""
ADS Bridge Module: Connects Python Flask backend with C ADS Executables
Provides seamless integration between MySQL data and C Algorithms:
  1. Linked List
  2. FIFO Queue
  3. LIFO Stack
  4. Priority Queue (Max-Heap)
  5. Merge Sort
  6. Binary Search
"""

import os
import subprocess
import json
import logging

logger = logging.getLogger(__name__)

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ALGO_DIR = os.path.join(BASE_DIR, 'algorithms')
BIN_DIR = os.path.join(ALGO_DIR, 'bin')

def get_exe_path(name):
    ext = '.exe' if os.name == 'nt' else ''
    path = os.path.join(BIN_DIR, f"{name}{ext}")
    if not os.path.exists(path):
        # Auto-compile if not already compiled
        compile_script = os.path.join(ALGO_DIR, 'compile_algorithms.py')
        if os.path.exists(compile_script):
            subprocess.run(['python', compile_script], capture_output=True)
    return path

def format_tasks_batch(tasks):
    """
    Format list of task dicts into batch string:
    id|title|priority|deadline|status;...
    """
    parts = []
    for t in tasks:
        # Sanitize delimiter '|' and ';'
        tid = t.get('task_id', 0)
        title = str(t.get('title', '')).replace('|', ' ').replace(';', ' ')
        prio = str(t.get('priority', 'Medium')).replace('|', ' ').replace(';', ' ')
        deadline = str(t.get('deadline', '')).replace('|', ' ').replace(';', ' ')
        status = str(t.get('status', 'Pending')).replace('|', ' ').replace(';', ' ')
        parts.append(f"{tid}|{title}|{prio}|{deadline}|{status}")
    return ";".join(parts)

def format_stack_batch(tasks):
    """
    Format completed tasks for stack:
    id|title|completed_at;...
    """
    parts = []
    for t in tasks:
        tid = t.get('task_id', 0)
        title = str(t.get('title', '')).replace('|', ' ').replace(';', ' ')
        comp = str(t.get('completed_at') or t.get('updated_at') or '2026-11-09').replace('|', ' ').replace(';', ' ')
        parts.append(f"{tid}|{title}|{comp}")
    return ";".join(parts)

def run_linked_list(tasks, action="batch", delete_id=None):
    exe = get_exe_path('linked_list')
    batch_str = format_tasks_batch(tasks)
    
    cmd = [exe, "batch", batch_str]
    if action == "delete" and delete_id:
        cmd.extend(["delete", str(delete_id)])
        
    try:
        res = subprocess.run(cmd, capture_output=True, text=True, check=True)
        return json.loads(res.stdout)
    except Exception as e:
        logger.error(f"Error running linked_list: {e}")
        return {
            "algorithm": "Linked List",
            "status": "error",
            "message": str(e),
            "node_count": len(tasks),
            "tasks": tasks
        }

def run_queue(tasks, action="batch"):
    exe = get_exe_path('queue')
    batch_str = format_tasks_batch(tasks)
    
    cmd = [exe, "batch", batch_str]
    if action == "dequeue":
        cmd.append("dequeue")
        
    try:
        res = subprocess.run(cmd, capture_output=True, text=True, check=True)
        return json.loads(res.stdout)
    except Exception as e:
        logger.error(f"Error running queue: {e}")
        return {
            "algorithm": "FIFO Queue",
            "status": "error",
            "message": str(e),
            "size": len(tasks),
            "tasks": tasks
        }

def run_stack(completed_tasks, action="batch"):
    exe = get_exe_path('stack')
    batch_str = format_stack_batch(completed_tasks)
    
    cmd = [exe, "batch", batch_str]
    if action == "pop":
        cmd.append("pop")
        
    try:
        res = subprocess.run(cmd, capture_output=True, text=True, check=True)
        return json.loads(res.stdout)
    except Exception as e:
        logger.error(f"Error running stack: {e}")
        return {
            "algorithm": "LIFO Stack",
            "status": "error",
            "message": str(e),
            "total_items": len(completed_tasks),
            "stack_history": completed_tasks
        }

def run_priority_queue(tasks, action="batch"):
    exe = get_exe_path('priority_queue')
    batch_str = format_tasks_batch(tasks)
    
    cmd = [exe, "batch", batch_str]
    if action == "extract":
        cmd.append("extract")
        
    try:
        res = subprocess.run(cmd, capture_output=True, text=True, check=True)
        return json.loads(res.stdout)
    except Exception as e:
        logger.error(f"Error running priority_queue: {e}")
        return {
            "algorithm": "Priority Queue",
            "status": "error",
            "message": str(e),
            "heap_size": len(tasks),
            "priority_ordered_tasks": tasks
        }

def run_merge_sort(tasks):
    exe = get_exe_path('merge_sort')
    batch_str = format_tasks_batch(tasks)
    
    cmd = [exe, "batch", batch_str]
    try:
        res = subprocess.run(cmd, capture_output=True, text=True, check=True)
        return json.loads(res.stdout)
    except Exception as e:
        logger.error(f"Error running merge_sort: {e}")
        return {
            "algorithm": "Merge Sort",
            "status": "error",
            "message": str(e),
            "task_count": len(tasks),
            "sorted_tasks": sorted(tasks, key=lambda x: str(x.get('deadline', '')))
        }

def run_binary_search(tasks, target_id):
    exe = get_exe_path('binary_search')
    batch_str = format_tasks_batch(tasks)
    
    cmd = [exe, "search", str(target_id), batch_str]
    try:
        res = subprocess.run(cmd, capture_output=True, text=True, check=True)
        return json.loads(res.stdout)
    except Exception as e:
        logger.error(f"Error running binary_search: {e}")
        return {
            "algorithm": "Binary Search",
            "status": "error",
            "message": str(e),
            "target_id": target_id,
            "found": False,
            "search_steps": []
        }

def get_source_code(filename):
    """Retrieve raw C source code for exhibition and demonstration."""
    path = os.path.join(ALGO_DIR, filename)
    if os.path.exists(path):
        with open(path, 'r', encoding='utf-8') as f:
            return f.read()
    return "// Source code not found"

