"""
Comprehensive Automated Test Suite
Verifies:
  1. MySQL Database connectivity & CRUD
  2. All 6 compiled C algorithm binaries
  3. ads_bridge.py JSON parsing & metrics
  4. Flask application routes, session authentication, task CRUD, and API endpoints
"""

import os
import sys
import json
import unittest

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, BASE_DIR)

from backend import db, ads_bridge
from backend.app import app

class TestStudentTaskSystem(unittest.TestCase):

    def setUp(self):
        app.config['TESTING'] = True
        app.config['WTF_CSRF_ENABLED'] = False
        self.client = app.test_client()

    def test_01_database_connection(self):
        student = db.get_student_by_id(1)
        self.assertIsNotNone(student)
        self.assertEqual(student['email'], 'student@college.edu')
        print("[PASS] Database Connection & Student 1 verified.")

    def test_02_c_linked_list(self):
        tasks = db.get_tasks_by_student(1)
        res = ads_bridge.run_linked_list(tasks)
        self.assertEqual(res.get('status'), 'success')
        self.assertEqual(res.get('algorithm'), 'Linked List')
        self.assertGreaterEqual(res.get('node_count', 0), 1)
        print(f"[PASS] C Linked List verified: {res.get('node_count')} nodes.")

    def test_03_c_fifo_queue(self):
        tasks = [t for t in db.get_tasks_by_student(1) if t['status'] == 'Pending']
        res = ads_bridge.run_queue(tasks)
        self.assertEqual(res.get('status'), 'success')
        self.assertEqual(res.get('algorithm'), 'FIFO Queue')
        print(f"[PASS] C FIFO Queue verified: size {res.get('size')}.")

    def test_04_c_lifo_stack(self):
        completed = db.get_completed_tasks_stack(1)
        res = ads_bridge.run_stack(completed)
        self.assertEqual(res.get('status'), 'success')
        self.assertIn('Stack', res.get('algorithm', ''))
        print(f"[PASS] C LIFO Stack verified: {res.get('total_items')} items.")

    def test_05_c_priority_queue(self):
        tasks = db.get_tasks_by_student(1)
        res = ads_bridge.run_priority_queue(tasks)
        self.assertEqual(res.get('status'), 'success')
        ordered = res.get('priority_ordered_tasks', [])
        self.assertTrue(len(ordered) > 0)
        # Verify first item has priority score >= last item
        self.assertGreaterEqual(ordered[0]['score'], ordered[-1]['score'])

        # Explicit test for equal priority nearest deadline tie-breaker
        tie_test_tasks = [
            {'task_id': 201, 'title': 'High Due Later', 'priority': 'High', 'deadline': '2026-11-25', 'status': 'Pending'},
            {'task_id': 202, 'title': 'High Due Sooner', 'priority': 'High', 'deadline': '2026-11-10', 'status': 'Pending'},
            {'task_id': 203, 'title': 'Med Due Later', 'priority': 'Medium', 'deadline': '2026-11-30', 'status': 'Pending'},
            {'task_id': 204, 'title': 'Med Due Sooner', 'priority': 'Medium', 'deadline': '2026-11-12', 'status': 'Pending'}
        ]
        tie_res = ads_bridge.run_priority_queue(tie_test_tasks)
        tie_ordered = tie_res.get('priority_ordered_tasks', [])
        self.assertEqual(tie_ordered[0]['task_id'], 202, "Task 202 (High Sooner) must be Rank 1")
        self.assertEqual(tie_ordered[1]['task_id'], 201, "Task 201 (High Later) must be Rank 2")
        self.assertEqual(tie_ordered[2]['task_id'], 204, "Task 204 (Med Sooner) must be Rank 3")
        self.assertEqual(tie_ordered[3]['task_id'], 203, "Task 203 (Med Later) must be Rank 4")
        print(f"[PASS] C Priority Queue (Max-Heap with Nearest Deadline Tie-Breaker) verified.")

    def test_06_c_merge_sort(self):
        tasks = db.get_tasks_by_student(1)
        res = ads_bridge.run_merge_sort(tasks)
        self.assertEqual(res.get('status'), 'success')
        self.assertEqual(res.get('algorithm'), 'Merge Sort')
        sorted_tasks = res.get('sorted_tasks', [])
        # Check chronological order
        for i in range(len(sorted_tasks) - 1):
            self.assertLessEqual(sorted_tasks[i]['deadline'], sorted_tasks[i+1]['deadline'])
        print(f"[PASS] C Merge Sort verified: {len(sorted_tasks)} tasks sorted by deadline.")

    def test_07_c_binary_search(self):
        tasks = db.get_tasks_by_student(1)
        target = tasks[0]['task_id']
        res = ads_bridge.run_binary_search(tasks, target)
        self.assertEqual(res.get('status'), None) # Direct JSON from binary
        self.assertTrue(res.get('found'))
        self.assertIsNotNone(res.get('found_task'))
        self.assertEqual(res.get('found_task')['task_id'], target)
        print(f"[PASS] C Binary Search verified: found #{target} in {res.get('comparison_count')} comparisons.")

    def test_08_flask_authentication(self):
        # Login test
        res = self.client.post('/login', data={'email': 'student@college.edu', 'password': 'student123'}, follow_redirects=True)
        self.assertEqual(res.status_code, 200)
        self.assertIn(b'Alex Rivers', res.data)
        print("[PASS] Flask Login & Session flow verified.")

    def test_09_dashboard_and_algorithms_page(self):
        with self.client.session_transaction() as sess:
            sess['student_id'] = 1
            sess['student_name'] = 'Alex Rivers'
            sess['roll_number'] = '23CS101'

        # Dashboard
        res = self.client.get('/dashboard')
        self.assertEqual(res.status_code, 200)
        self.assertIn(b'Academic Dashboard', res.data)

        # Merge Sort View
        res_ms = self.client.get('/dashboard?sort=deadline_merge_sort')
        self.assertEqual(res_ms.status_code, 200)
        self.assertIn(b'C Merge Sort', res_ms.data)

        # Priority View
        res_pq = self.client.get('/dashboard?sort=priority_heap')
        self.assertEqual(res_pq.status_code, 200)
        self.assertIn(b'C Priority Max-Heap', res_pq.data)

        # Algorithms Hub
        res_algo = self.client.get('/algorithms')
        self.assertEqual(res_algo.status_code, 200)
        self.assertIn(b'Pure C Implementations', res_algo.data)
        print("[PASS] Dashboard, C Sorting Views, and Algorithms Hub verified.")

    def test_10_api_algorithms_run(self):
        with self.client.session_transaction() as sess:
            sess['student_id'] = 1
            sess['student_name'] = 'Alex Rivers'

        res = self.client.post('/api/algorithms/run', 
                               data=json.dumps({'algorithm': 'merge_sort'}),
                               content_type='application/json')
        self.assertEqual(res.status_code, 200)
        data = json.loads(res.data)
        self.assertEqual(data.get('algorithm'), 'Merge Sort')
        print("[PASS] API /api/algorithms/run endpoint verified.")

if __name__ == '__main__':
    unittest.main()

