# Comprehensive Viva & Examination Preparation Guide
## Advanced Data Structures (ADS) — Project-Based Learning (PBL)

This document contains 30 in-depth oral examination questions and authoritative technical answers covering all aspects of the **Student Task and Deadline Management System**.

---

### Part 1: Project Architecture & System Integration

#### Q1. What is the fundamental architecture of this PBL project?
**Answer**:
The project is built on a 4-tier hybrid architecture:
1. **Presentation Layer**: Responsive frontend designed with HTML5, CSS3 (Custom Properties, Flexbox, Grid), and vanilla JavaScript with Chart.js for data visualizations.
2. **Application Layer**: Python Flask backend (`backend/app.py`) managing routing, session authentication, and REST APIs.
3. **Data Persistence Layer**: MySQL 8.0 relational database (`student_task_db`) maintaining normalized tables (`Students`, `Subjects`, `Tasks`) with foreign keys and cascading updates.
4. **Algorithmic Engine (Core ADS Layer)**: Six distinct native C source code files (`algorithms/*.c`), compiled into machine-code Windows binaries (`algorithms/bin/*.exe`). A specialized subprocess bridge (`backend/ads_bridge.py`) serializes relational task data, pipes it to the C executables, and parses structured JSON output streams.

#### Q2. Why implement the ADS algorithms in C language instead of using Python's built-in data structures?
**Answer**:
1. **Pedagogical Integrity**: Python abstracts away memory allocation, pointers, and memory layout (e.g. `list.sort()` is Timsort in CPython, `list` is a dynamic array of pointers). Implementing in C requires explicit control over pointers (`TaskNode* next`), manual dynamic allocation (`malloc` and `free`), pointer arithmetic, and explicit recursive call-stack manipulation.
2. **Determinism & Performance**: Pure C compiles directly into native CPU instructions without runtime interpretation overhead, allowing students to demonstrate true algorithmic time and space complexity without garbage collector interference.
3. **PBL Requirement Fulfillment**: The project specification mandates separate C language files without replacing them with Python abstractions.

#### Q3. How does the Flask backend communicate with the compiled C binaries?
**Answer**:
The integration is handled by [`backend/ads_bridge.py`](file:///c:/Users/admin/Desktop/ADS-PBL/backend/ads_bridge.py) using the OS subprocess interface:
1. Flask queries the MySQL database for the logged-in student's tasks.
2. The bridge serializes the records into a delimited batch string:
   `"task_id|title|priority|deadline|status;..."`
3. The bridge invokes the respective C binary (e.g., `algorithms/bin/merge_sort.exe batch "<data>"`) via `subprocess.run()`.
4. The C binary parses the input, executes the algorithm, formats the output into standard JSON, and writes to `stdout`.
5. The Python bridge captures `stdout`, validates the JSON object, and injects the sorted/searched data directly into the web view and REST responses.

---

### Part 2: Algorithm 1 — Dynamic Singly Linked List (`linked_list.c`)

#### Q4. What is a Singly Linked List and what are its node components?
**Answer**:
A Singly Linked List is a linear dynamic data structure composed of nodes allocated non-contiguously in heap memory. In `algorithms/linked_list.c`:
```c
typedef struct TaskNode {
    int task_id;
    char title[MAX_TITLE];
    char priority[MAX_PRIORITY];
    char deadline[MAX_DATE];
    char status[MAX_PRIORITY];
    struct TaskNode* next; // Self-referential pointer
} TaskNode;
```
Each node contains task data fields and a single forward reference pointer (`next`) pointing to the successive node, terminating with `NULL`.

#### Q5. What are the time complexities for insertion, deletion, and search in your linked list?
**Answer**:
- **Insertion at Head**: $\mathcal{O}(1)$ — create new node, point `newNode->next = head`, set `head = newNode`.
- **Insertion at Tail**: $\mathcal{O}(1)$ — our `LinkedList` control struct maintains both `head` and `tail` pointers (`tail->next = newNode; tail = newNode`).
- **Deletion by Task ID**: $\mathcal{O}(n)$ — requires traversing from `head` to locate the target node and maintain a pointer to the preceding node (`prev`) to rewire `prev->next = current->next`.
- **Linear Search / Traversal**: $\mathcal{O}(n)$ — sequential element visit from head to NULL.
- **Space Complexity**: $\mathcal{O}(n)$ — dynamic node allocation on the heap plus pointer overhead ($\approx 8$ bytes per pointer on 64-bit systems).

#### Q6. What is the advantage of a Linked List over a static array for storing academic tasks?
**Answer**:
1. **Dynamic Resizing**: Arrays require contiguous memory allocation with predefined fixed capacity (`MAX_TASKS`). A linked list grows and shrinks dynamically as tasks are created or deleted.
2. **No Memory Wastage**: Unused array slots consume memory; linked lists allocate exactly one node per task.
3. **No Contiguity Requirement**: Unlike arrays, linked list nodes do not require large contiguous blocks of free memory, preventing allocation failures when heap memory is fragmented.

---

### Part 3: Algorithm 2 — Circular FIFO Queue (`queue.c`)

#### Q7. What is a Circular Queue and why is it preferred over a simple linear array queue?
**Answer**:
In a simple linear array queue, as elements are enqueued at `rear` and dequeued from `front`, the `front` index continuously increments toward the end of the array. Even if earlier slots become vacant, `rear == MAX - 1` signals a false "Queue Full" condition (**Queue Drift**).
A **Circular Queue** wraps index arithmetic modulo the array capacity:
$$\text{rear} = (\text{rear} + 1) \pmod{\text{MAX\_QUEUE}}$$
$$\text{front} = (\text{front} + 1) \pmod{\text{MAX\_QUEUE}}$$
This reuses vacated front slots, ensuring optimal memory utilization in $\mathcal{O}(1)$ time.

#### Q8. How do you distinguish between Queue Empty and Queue Full conditions in `queue.c`?
**Answer**:
In `algorithms/queue.c`, we track an explicit `size` counter:
- **Queue Empty**: `size == 0`
- **Queue Full**: `size == MAX_QUEUE`
Alternatively, without a size counter, an array of size $N$ uses pointer arithmetic:
- Empty: `front == rear + 1` or `front == -1`
- Full: `(rear + 1) % N == front`

#### Q9. How is the FIFO queue applied in the student task workflow?
**Answer**:
It acts as the **Pending Task Dispatch Queue**. When students have multiple academic deliverables scheduled for the day, tasks enter the queue in chronological arrival order (FIFO). The student executes the front task first, mimicking real-world batch processing and operating system CPU job scheduling.

---

### Part 4: Algorithm 3 — LIFO Stack (`stack.c`)

#### Q10. What is a Stack and what is its governing discipline?
**Answer**:
A Stack is a restricted linear data structure adhering to the **Last-In, First-Out (LIFO)** discipline. Insertion and removal are permitted only at one designated end called the `top` of the stack.

#### Q11. How are stack operations implemented in `stack.c`?
**Answer**:
- **Push**: Increment `top` and assign the completed task:
  `s->top++; s->data[s->top] = item;` $\implies \mathcal{O}(1)$
- **Pop**: Copy `s->data[s->top]` to output and decrement `s->top--`: $\implies \mathcal{O}(1)$
- **Peek**: Return `s->data[s->top]` without modifying `top`: $\implies \mathcal{O}(1)$
- **Underflow Condition**: `top == -1`
- **Overflow Condition**: `top == MAX_STACK - 1`

#### Q12. How does the Stack power the "Undo Completed Task" feature?
**Answer**:
When a student marks an assignment complete, the application pushes that task ID, title, and timestamp onto the LIFO stack. If the student marked it accidentally, invoking "Undo" pops the top of the stack. Because of LIFO ordering, the *most recently completed task* is restored first, exactly matching user expectation.

---

### Part 5: Algorithm 4 — Binary Max-Heap Priority Queue (`priority_queue.c`)

#### Q13. What is a Priority Queue and why implement it as a Binary Heap instead of an array?
**Answer**:
A Priority Queue is an abstract data type where each element has an assigned priority, and elements with higher priority are served before elements with lower priority.
- In an **unsorted array**: Insertion is $\mathcal{O}(1)$, but finding/extracting the maximum is $\mathcal{O}(n)$.
- In a **sorted array**: Finding maximum is $\mathcal{O}(1)$, but insertion requires $\mathcal{O}(n)$ shifts.
- In a **Binary Heap**: Both insertion and extraction take $\mathcal{O}(\log n)$, providing the optimal balance for dynamic scheduling.

#### Q14. What are the two essential properties of a Binary Max-Heap?
**Answer**:
1. **Shape Property (Complete Binary Tree)**: Every level of the tree is completely filled, except possibly the last level, which is filled strictly from left to right. This allows the tree to be mapped efficiently into a 1D array without child pointers.
2. **Heap Property (Max-Heap Order)**: For every node $i$ other than the root:
   $$\text{Key}(\text{Parent}(i)) \ge \text{Key}(i)$$
   The element with the highest priority is always stored at index 0 (root).

#### Q15. How are tree parent-child relationships calculated in a 1D array?
**Answer**:
For any zero-indexed element at index $i$:
- **Parent**: $\lfloor(i - 1) / 2\rfloor$
- **Left Child**: $2i + 1$
- **Right Child**: $2i + 2$

#### Q16. Explain Sift-Up and Sift-Down operations.
**Answer**:
- **Sift-Up (Heapify-Up)**: Used during `insert()`. The new task is placed at the end of the heap array (index `size`). It is repeatedly compared with its parent; if its priority score exceeds the parent's, they are swapped. This repeats upward along the tree branch. Maximum swaps: $\log_2(n) \implies \mathcal{O}(\log n)$.
- **Sift-Down (Heapify-Down)**: Used during `extractMax()`. The root (highest priority) is removed and replaced with the last element in the array (`tree[size - 1]`). The element is compared with its largest child; if smaller, it swaps with that child. This repeats downward until the max-heap invariant is restored. Maximum swaps: $\log_2(n) \implies \mathcal{O}(\log n)$.

#### Q17. How are task priorities and tie-breakers implemented in `priority_queue.c`?
**Answer**:
A multi-tier composite comparison function `hasHigherPriority(a, b)` governs heap order:
1. **Primary Key — Numerical Priority Weight**:
   - **High Priority** $\implies$ Score: `3` (highest precedence, sits at root)
   - **Medium Priority** $\implies$ Score: `2`
   - **Low Priority** $\implies$ Score: `1`
2. **Secondary Key — Nearest Deadline Tie-Breaker**:
   - If two tasks have equal priority scores (e.g., both High or both Medium), their deadlines are compared chronologically (`strcmp(a->deadline, b->deadline) < 0`). The task with the **nearest (earliest) deadline** takes precedence and appears first.
3. **Tertiary Key — Determinism**:
   - If priorities and deadlines are identical, smaller `task_id` appears first.
This guarantees that among tasks of the same importance, the most imminent deadline receives immediate attention.

---

### Part 6: Algorithm 5 — Divide & Conquer Merge Sort (`merge_sort.c`)

#### Q18. Explain the Divide, Conquer, and Combine steps of Merge Sort.
**Answer**:
1. **Divide**: Calculate the middle index $mid = left + (right - left) / 2$ and recursively split the array into two sub-problems: left half $[left \dots mid]$ and right half $[mid + 1 \dots right]$.
2. **Conquer**: Recursively call `mergeSort()` on both halves until base case is reached ($left \ge right$, sub-array of length 1 is already sorted).
3. **Combine (Merge)**: Allocate temporary arrays $L$ and $R$. Use two pointers to compare elements in $L$ and $R$ and merge them in non-decreasing order back into the primary array.

#### Q19. State and solve the recurrence relation for Merge Sort.
**Answer**:
The recurrence relation is:
$$T(n) = 2T\left(\frac{n}{2}\right) + \mathcal{O}(n)$$
where $2T(n/2)$ represents sorting the two halves, and $\mathcal{O}(n)$ represents the linear merging subroutine.
By the **Master Theorem**:
- $a = 2, b = 2, f(n) = \mathcal{O}(n)$
- $\log_b(a) = \log_2(2) = 1$
- Since $f(n) = \Theta(n^{\log_b a}) = \Theta(n^1)$, Case 2 of the Master Theorem applies:
$$T(n) = \Theta(n^{\log_b a} \log n) = \mathcal{O}(n \log n)$$
Merge Sort guarantees $\mathcal{O}(n \log n)$ time in **Best, Average, and Worst cases**.

#### Q20. Is Merge Sort a stable sorting algorithm? Why does stability matter here?
**Answer**:
**Yes, Merge Sort is stable.** In `merge_sort.c`:
```c
if (compareDeadlines(L[i].deadline, R[j].deadline) <= 0) {
    arr[k] = L[i++];
} else {
    arr[k] = R[j++];
}
```
The `<=` condition ensures that when two tasks have identical deadlines, their relative pre-existing order is preserved. Stability prevents erratic reordering of assignments belonging to the same due date.

#### Q21. What is the auxiliary space complexity of Merge Sort?
**Answer**:
$\mathcal{O}(n)$ auxiliary space. The merge subroutine must allocate temporary arrays ($L$ and $R$) of size $n_1 + n_2 = n$ to hold sub-elements during comparison before copying them back.

---

### Part 7: Algorithm 6 — Binary Search on Task IDs (`binary_search.c`)

#### Q22. What is the mandatory precondition for Binary Search?
**Answer**:
The collection must be **monotonically sorted** by the search key (`task_id`). In `algorithms/binary_search.c`, the array is pre-sorted before search execution.

#### Q23. Why use `mid = low + (high - low) / 2` instead of `mid = (low + high) / 2`?
**Answer**:
To prevent **arithmetic integer overflow**. If `low` and `high` are large positive integers, `low + high` can exceed the maximum representable 32-bit signed integer value ($2^{31} - 1 = 2,147,483,647$), wrapping into a negative number and causing an out-of-bounds memory segmentation fault. The formula `low + (high - low) / 2` is mathematically equivalent and never overflows.

#### Q24. Derive the time complexity of Binary Search.
**Answer**:
At each iteration, the search space is divided in half:
- After iteration 1: $n / 2$ elements remain.
- After iteration 2: $n / 4$ elements remain.
- After iteration $k$: $n / 2^k$ elements remain.
The algorithm terminates when the search space reduces to 1:
$$\frac{n}{2^k} = 1 \implies n = 2^k \implies k = \log_2(n)$$
Thus, Binary Search takes at most $\lceil\log_2(n)\rceil + 1$ comparisons:
- **Best Case**: $\mathcal{O}(1)$ (target matches initial midpoint).
- **Average & Worst Case**: $\mathcal{O}(\log n)$.
- **Space Complexity**: $\mathcal{O}(1)$ auxiliary space for the iterative implementation.

---

### Part 8: Database Design, Security & Web Engineering

#### Q25. Explain the normalization level of `student_task_db`.
**Answer**:
The database satisfies **Third Normal Form (3NF)**:
1. **1NF**: All columns contain atomic (indivisible) values; no repeating groups.
2. **2NF**: In 1NF, and all non-key attributes are fully functionally dependent on the primary key (no partial dependencies).
3. **3NF**: In 2NF, and no non-key attribute is transitively dependent on the primary key. `Tasks` stores only foreign keys `student_id` and `subject_id`, eliminating duplicate student/subject metadata.

#### Q26. How are relational constraints configured?
**Answer**:
- `Tasks.student_id` $\rightarrow$ `Students.student_id` with `ON DELETE CASCADE ON UPDATE CASCADE`: Deleting a student automatically purges all their associated tasks.
- `Tasks.subject_id` $\rightarrow$ `Subjects.subject_id` with `ON DELETE SET NULL ON UPDATE CASCADE`: Deleting an academic subject preserves the student's task records while nullifying the subject reference.

#### Q27. How are student passwords secured?
**Answer**:
Passwords are never stored in plain text. They are hashed using **Werkzeug's PBKDF2/scrypt key-derivation function** with a cryptographic salt. During authentication, `check_password_hash()` compares the candidate password hash with the database digest in constant time, mitigating timing attacks.

#### Q28. How does the application prevent SQL Injection attacks?
**Answer**:
All SQL queries in `backend/db.py` use **parameterized queries** (prepared statements) with `pymysql`:
```python
cur.execute("SELECT * FROM Students WHERE email = %s", (email,))
```
User inputs are treated strictly as literal data, preventing malicious SQL code injection.

#### Q29. How does the upcoming deadline notification system work?
**Answer**:
MySQL datetime arithmetic identifies urgent assignments:
```sql
SELECT * FROM Tasks 
WHERE student_id = %s AND status = 'Pending' 
  AND deadline BETWEEN NOW() AND DATE_ADD(NOW(), INTERVAL 48 HOUR);
```
Tasks due within 48 hours trigger dynamic warning indicators: a pulsating bell icon in the top navbar and a high-priority alert banner at the top of the dashboard.

#### Q30. Summarize the Big-O performance across all six algorithms.
**Answer**:

| Algorithm | Structure | Best Case | Average Case | Worst Case | Space |
| :--- | :--- | :---: | :---: | :---: | :---: |
| **1. Linked List** | Dynamic Nodes | $\mathcal{O}(1)$ | $\mathcal{O}(n)$ | $\mathcal{O}(n)$ | $\mathcal{O}(n)$ |
| **2. FIFO Queue** | Circular Array | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(\text{CAP})$ |
| **3. LIFO Stack** | Top-indexed Array | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(\text{CAP})$ |
| **4. Priority Queue** | Binary Max-Heap | $\mathcal{O}(1)$ | $\mathcal{O}(\log n)$ | $\mathcal{O}(\log n)$ | $\mathcal{O}(n)$ |
| **5. Merge Sort** | Recursion Subarrays | $\mathcal{O}(n \log n)$ | $\mathcal{O}(n \log n)$ | $\mathcal{O}(n \log n)$ | $\mathcal{O}(n)$ |
| **6. Binary Search** | Sorted Array | $\mathcal{O}(1)$ | $\mathcal{O}(\log n)$ | $\mathcal{O}(\log n)$ | $\mathcal{O}(1)$ |

