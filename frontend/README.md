# Frontend Architecture & Design System

The frontend of the **Student Task and Deadline Management System** is engineered using pure semantic HTML5, modern CSS3 (Custom Variables, Flexbox, Grid), and vanilla JavaScript with Chart.js.

## Design Highlights
- **Color Palette**:
  - Primary Blue: `#2563eb`, `#1d4ed8`
  - Purple Accent: `#7c3aed`, `#4f46e5`
  - Backgrounds: `#ffffff` (card surface), `#f8fafc` (main canvas), `#0f172a` (sidebar/terminal)
- **Priority Visual Coding**:
  - High Priority: `#e11d48` (Rose/Red)
  - Medium Priority: `#d97706` (Amber/Orange)
  - Low Priority: `#059669` (Emerald/Green)
- **Responsive Layout**:
  - Fixed / collapsible sidebar on mobile devices (<768px).
  - Multi-column grid adapting between desktop and tablets.

## Key Frontend Components
1. **Student Dashboard (`templates/dashboard.html`)**:
   - Urgent deadline notifications banner with countdown calculations.
   - 4 metric cards: Total Tasks, Completed Rate, Pending Tasks, High Priority Tasks.
   - Live Chart.js donut chart (Completion Rate) and bar chart (Priority Breakdown).
   - Dynamic monthly deadline calendar highlighting task due dates.
   - Interactive C algorithm quick-action toolbar.
   - Comprehensive task cards with subject tags, status toggle, edit, and delete triggers.
   - LIFO completed tasks stack visualizer.
2. **ADS Algorithms Playground (`templates/algorithms.html`)**:
   - Tabbed layout covering all 6 mandatory C data structures.
   - Interactive sandbox with inputs and execution buttons.
   - Live visual diagram (linked list node chain, circular queue slots, stack tower, max-heap table, merge sort tree, binary search divide-and-conquer trace).
   - Embedded terminal window displaying raw JSON output from compiled C binaries.
   - Big-O time and space complexity breakdown.
   - Integrated C Source Code Viewer with syntax styling and one-click copy.
3. **Authentication Pages (`templates/login.html` & `templates/register.html`)**:
   - Academic portal styling with department and semester selection.
   - Quick one-click demo credentials autofill pill.

