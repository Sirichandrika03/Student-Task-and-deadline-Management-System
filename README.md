# STUDENT TASK AND DEADLINE MANAGEMENT SYSTEM
### Advanced Data Structures (ADS) - College PBL Project

A complete, production-grade academic task management web application built for **Advanced Data Structures (ADS)**. This system features native **C language** implementations for all six mandatory data structures and algorithms, integrated seamlessly with a **Python Flask** backend and a **MySQL** database.

---

## 🚀 Key Features

- **Student Authentication & Sessions**: Student registration, login, secure password hashing, and session management.
- **Academic Dashboard**: High-level task analytics, completion progress bar, overdue indicators, and upcoming deadline notifications within 48 hours.
- **Interactive Visual Charts**: Chart.js doughnut chart for completion rate and bar chart for priority distribution.
- **Deadline Calendar**: Monthly calendar dynamically highlighting academic deadlines with color-coded priority dots.
- **Complete Task CRUD**:
  - Add tasks with title, description, priority (High, Medium, Low), deadline datetime, and subject.
  - Edit and update existing tasks.
  - Delete tasks (with confirmation).
  - Mark tasks completed or reopen as pending.
- **Search & Multi-Filter Toolbar**: Search by keyword; filter by Status (All/Pending/Completed), Priority, or Subject.
- **Dedicated ADS Algorithms Demonstration Hub**:
  - Full interactive workbench for all 6 C algorithms.
  - Step-by-step visual diagrams (Linked List chains, circular queue slots, stack tower, heap sort schedule, merge steps, binary search trace).
  - Live execution of compiled C binaries via subprocess bridge.
  - Mathematical Big-O time and space complexity tables.
  - Integrated C Source Code Viewer with syntax formatting and one-click copy.

---

## 🧠 The 6 Mandatory ADS Algorithms (C Implementations)

All six algorithms are implemented in separate C files under the `algorithms/` folder and compiled to native `.exe` binaries:

| # | Algorithm | C Source File | Core ADS Concepts | Time Complexity |
|---|---|---|---|---|
| **1** | **Linked List** | [`algorithms/linked_list.c`](file:///c:/Users/admin/Desktop/ADS-PBL/algorithms/linked_list.c) | Dynamic heap node allocation (`malloc`/`free`), pointer manipulation, dynamic insertion & deletion. | Insert: $\mathcal{O}(1)$, Delete: $\mathcal{O}(n)$, Traverse: $\mathcal{O}(n)$ |
| **2** | **FIFO Queue** | [`algorithms/queue.c`](file:///c:/Users/admin/Desktop/ADS-PBL/algorithms/queue.c) | Circular queue with modulo arithmetic (`(rear + 1) % MAX`), front/rear pointers, FIFO discipline. | Enqueue: $\mathcal{O}(1)$, Dequeue: $\mathcal{O}(1)$ |
| **3** | **LIFO Stack** | [`algorithms/stack.c`](file:///c:/Users/admin/Desktop/ADS-PBL/algorithms/stack.c) | Top pointer management, push (completion history), pop (undo complete), peek top. | Push: $\mathcal{O}(1)$, Pop: $\mathcal{O}(1)$, Peek: $\mathcal{O}(1)$ |
| **4** | **Priority Queue** | [`algorithms/priority_queue.c`](file:///c:/Users/admin/Desktop/ADS-PBL/algorithms/priority_queue.c) | Binary Max-Heap represented in 1D array. Sift-Up on insert, Sift-Down on extract-max. | Insert: $\mathcal{O}(\log n)$, Extract: $\mathcal{O}(\log n)$, Sort: $\mathcal{O}(n \log n)$ |
| **5** | **Merge Sort** | [`algorithms/merge_sort.c`](file:///c:/Users/admin/Desktop/ADS-PBL/algorithms/merge_sort.c) | Divide & Conquer recursive splitting, linear two-pointer merge subroutine. Chronological deadline sorting. | Best / Avg / Worst: $\mathcal{O}(n \log n)$ |
| **6** | **Binary Search** | [`algorithms/binary_search.c`](file:///c:/Users/admin/Desktop/ADS-PBL/algorithms/binary_search.c) | Pre-sorted keys, midpoint calculation, halving of candidate space. Trace low, mid, high. | Best: $\mathcal{O}(1)$, Avg / Worst: $\mathcal{O}(\log n)$ |

---

## 📁 Project Folder Structure

```
ADS-PBL/
├── algorithms/                  # C Algorithm Source Codes & Compilers
│   ├── linked_list.c            # Algorithm 1: Dynamic Singly Linked List
│   ├── queue.c                  # Algorithm 2: Circular FIFO Queue
│   ├── stack.c                  # Algorithm 3: LIFO Completion Stack
│   ├── priority_queue.c         # Algorithm 4: Binary Max-Heap Priority Queue
│   ├── merge_sort.c             # Algorithm 5: Divide & Conquer Merge Sort
│   ├── binary_search.c          # Algorithm 6: Binary Search on Task IDs
│   ├── compile_algorithms.py    # Automatic C compiler script
│   └── bin/                     # Compiled native executables (*.exe)
├── backend/                     # Python Flask Server & Subprocess Bridge
│   ├── app.py                   # Main Flask application & routes
│   ├── db.py                    # MySQL relational connection & CRUD helpers
│   └── ads_bridge.py            # Bridge invoking C executables with JSON I/O
├── database/                    # Database Scripts & Seed Data
│   ├── schema.sql               # Complete SQL schema (Students, Subjects, Tasks)
│   └── init_db.py               # Database initialisation & sample data seeding
├── documentation/               # Comprehensive College PBL Documentation
│   └── PROJECT_REPORT.md        # Technical PBL report, ER diagram & algorithm proofs
├── frontend/                    # Frontend Architecture Notes
│   └── README.md                # Design system & component documentation
├── static/                      # Static Assets
│   ├── css/
│   │   └── style.css            # Custom modern blue/purple theme stylesheet
│   └── js/
│       └── main.js              # UI interaction, modal logic, calendar, Chart.js
├── templates/                   # Jinja2 HTML Templates
│   ├── base.html                # Sidebar layout, navbar & alert messages
│   ├── dashboard.html           # Main academic dashboard & task cards
│   ├── algorithms.html          # ADS Algorithms Hub & Interactive Playground
│   ├── login.html               # Student login page with demo autofill
│   ├── register.html            # Student registration page
│   ├── 404.html                 # 404 error page
│   └── 500.html                 # 500 server error page
├── tools/                       # C Compilers (Tiny C Compiler x64)
│   └── tcc/                     # Bundled standalone C99 compiler
├── run.py                       # Root launcher (compiles C & starts Flask server)
└── README.md                    # Project documentation
```

---

## ⚙️ Installation & Setup Instructions

### Prerequisites
1. **Python 3.10+** (tested on Python 3.15)
2. **MySQL Server 8.0+**
3. **C Compiler**: Bundled standalone Tiny C Compiler (`tools/tcc/tcc.exe`) is included, or any GCC / MinGW installation in PATH.

### Step 1: Install Python Dependencies
```bash
pip install Flask PyMySQL mysql-connector-python werkzeug
```

### Step 2: Configure & Initialize Database
The default configuration connects to local MySQL:
- Host: `127.0.0.1`
- Port: `3306`
- User: `root`
- Password: `Siri-25r11a62e6` (or set via environment variable `DB_PASS`)
- Database: `student_task_db`

To create the database and seed realistic demo records:
```bash
python database/init_db.py
```

### Step 3: Compile C Algorithm Binaries
To compile all six C source files into executable machine binaries:
```bash
python algorithms/compile_algorithms.py
```
This generates `linked_list.exe`, `queue.exe`, `stack.exe`, `priority_queue.exe`, `merge_sort.exe`, and `binary_search.exe` in `algorithms/bin/`.

### Step 4: Run the Web Application
```bash
python run.py
```
or
```bash
python backend/app.py
```

Open your browser and navigate to:
**http://127.0.0.1:5000**

---

## 🔑 Default Demo Student Credentials

You can sign in immediately using the pre-seeded account or click the autofill button on the login screen:
- **Email**: `student@college.edu`
- **Password**: `student123`
- **Roll Number**: `23CS101`
- **Department**: Computer Science & Engineering (Semester 5)

*(Or register a new student account at `/register`)*

---

## 🖥️ Running C Algorithms in Interactive Console Mode

Each C file can also be run independently from the terminal in interactive demo mode:

```bash
# 1. Linked List Interactive Console
.\algorithms\bin\linked_list.exe --interactive

# 2. Circular FIFO Queue Interactive Console
.\algorithms\bin\queue.exe --interactive

# 3. LIFO Task Stack Interactive Console
.\algorithms\bin\stack.exe --interactive

# 4. Max-Heap Priority Queue Interactive Console
.\algorithms\bin\priority_queue.exe --interactive

# 5. Merge Sort Interactive Console
.\algorithms\bin\merge_sort.exe --interactive

# 6. Binary Search Interactive Console
.\algorithms\bin\binary_search.exe --interactive
```

---

## 🎓 PBL Verification & Testing Highlights

- **C Algorithms**: Verified that C code executes directly (not simulated in Python).
- **Merge Sort**: Verified that pressing **"C Merge Sort Deadlines"** reorganizes task cards chronologically with $\mathcal{O}(n \log n)$ guarantee.
- **Priority Queue**: Verified that pressing **"C Priority Max-Heap"** places High-priority academic tasks at the top.
- **Binary Search**: Modal dialog executes binary search in C and prints the exact low, mid, high trace table.
- **Interactive Hub**: The `/algorithms` page provides live sandboxes to test operations and inspect the actual C source code.

