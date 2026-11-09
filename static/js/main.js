/**
 * STUDENT TASK AND DEADLINE MANAGEMENT SYSTEM
 * Advanced Data Structures (ADS) - PBL Project
 * Client-Side JavaScript: UI Interactions, Modals, Dynamic Calendar & Chart.js
 */

document.addEventListener("DOMContentLoaded", function () {
    initNavigation();
    initNotificationFlyout();
    initModalEscClose();
});

/* ---------------- Navigation & Mobile Sidebar ---------------- */
function initNavigation() {
    const sidebarToggle = document.getElementById("sidebarToggle");
    const sidebar = document.getElementById("sidebar");

    if (sidebarToggle && sidebar) {
        sidebarToggle.addEventListener("click", function () {
            sidebar.classList.toggle("open");
        });

        // Close sidebar when clicking outside on mobile
        document.addEventListener("click", function (e) {
            if (window.innerWidth <= 768 && sidebar.classList.contains("open")) {
                if (!sidebar.contains(e.target) && !sidebarToggle.contains(e.target)) {
                    sidebar.classList.remove("open");
                }
            }
        });
    }
}

/* ---------------- Notification Bell Dropdown ---------------- */
function initNotificationFlyout() {
    const bell = document.getElementById("notifBell");
    const flyout = document.getElementById("notifFlyout");

    if (bell && flyout) {
        bell.addEventListener("click", function (e) {
            e.stopPropagation();
            flyout.classList.toggle("open");
        });

        document.addEventListener("click", function (e) {
            if (!flyout.contains(e.target) && !bell.contains(e.target)) {
                flyout.classList.remove("open");
            }
        });
    }
}

/* ---------------- Modal Controls ---------------- */
function initModalEscClose() {
    document.addEventListener("keydown", function (e) {
        if (e.key === "Escape") {
            closeAddTaskModal();
            closeEditTaskModal();
            closeAddSubjectModal();
            closeBinarySearchModal();
        }
    });
}

function openAddTaskModal() {
    const modal = document.getElementById("addTaskModal");
    if (modal) modal.classList.add("open");
}

function closeAddTaskModal() {
    const modal = document.getElementById("addTaskModal");
    if (modal) modal.classList.remove("open");
}

function openEditTaskModal(id, title, description, priority, deadlineIso, subjectId) {
    const modal = document.getElementById("editTaskModal");
    const form = document.getElementById("editTaskForm");
    const idSpan = document.getElementById("editTaskModalId");

    if (modal && form) {
        form.action = `/tasks/edit/${id}`;
        idSpan.innerText = `#${id}`;

        document.getElementById("edit_title").value = title || "";
        document.getElementById("edit_description").value = description || "";
        document.getElementById("edit_priority").value = priority || "Medium";

        if (deadlineIso) {
            // Cut off seconds or timezone for datetime-local input (YYYY-MM-DDTHH:MM)
            const cleanIso = deadlineIso.substring(0, 16);
            document.getElementById("edit_deadline").value = cleanIso;
        }

        if (subjectId) {
            document.getElementById("edit_subject_id").value = subjectId;
        }

        modal.classList.add("open");
    }
}

function closeEditTaskModal() {
    const modal = document.getElementById("editTaskModal");
    if (modal) modal.classList.remove("open");
}

function openAddSubjectModal() {
    const modal = document.getElementById("addSubjectModal");
    if (modal) modal.classList.add("open");
}

function closeAddSubjectModal() {
    const modal = document.getElementById("addSubjectModal");
    if (modal) modal.classList.remove("open");
}

function openBinarySearchModal() {
    const modal = document.getElementById("binarySearchModal");
    if (modal) modal.classList.add("open");
}

function closeBinarySearchModal() {
    const modal = document.getElementById("binarySearchModal");
    if (modal) modal.classList.remove("open");
}

/* ---------------- C Binary Search Execution (AJAX) ---------------- */
function executeBinarySearch() {
    const targetInput = document.getElementById("bsTargetId");
    const targetId = parseInt(targetInput.value);

    if (!targetId || isNaN(targetId)) {
        alert("Please enter a valid numeric Task ID to search.");
        return;
    }

    const loading = document.getElementById("bsLoading");
    const results = document.getElementById("bsResults");
    const summaryBar = document.getElementById("bsSummaryBar");
    const tableBody = document.querySelector("#bsStepsTable tbody");
    const foundDetails = document.getElementById("bsFoundDetails");

    loading.style.display = "block";
    results.style.display = "none";

    fetch("/api/algorithms/run", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
            algorithm: "binary_search",
            target_id: targetId
        })
    })
    .then(res => res.json())
    .then(data => {
        loading.style.display = "none";
        results.style.display = "block";

        if (data.found && data.found_task) {
            summaryBar.className = "alert alert-success";
            summaryBar.innerHTML = `
                <i class="fa-solid fa-circle-check"></i>
                <strong>SUCCESS:</strong> Task ID #${targetId} found at sorted array index <code>${data.found_task.index}</code> in <strong>${data.comparison_count}</strong> divide-and-conquer steps. Time Complexity: <strong>O(log ${data.total_tasks})</strong>.
            `;
            foundDetails.innerHTML = `
                <div class="task-card" style="border-left-color: var(--primary-blue);">
                    <h4><strong>#${data.found_task.task_id}</strong> - ${data.found_task.title}</h4>
                    <p>Priority: <span class="badge-priority priority-${data.found_task.priority.toLowerCase()}">${data.found_task.priority}</span> | Deadline: ${data.found_task.deadline} | Status: ${data.found_task.status}</p>
                </div>
            `;
        } else {
            summaryBar.className = "alert alert-danger";
            summaryBar.innerHTML = `
                <i class="fa-solid fa-circle-xmark"></i>
                <strong>NOT FOUND:</strong> Task ID #${targetId} does not exist in the collection. Concluded after <strong>${data.comparison_count}</strong> comparisons. Time Complexity: <strong>O(log ${data.total_tasks})</strong>.
            `;
            foundDetails.innerHTML = `<p class="text-muted text-center">No record matching ID #${targetId}.</p>`;
        }

        // Render search steps
        tableBody.innerHTML = "";
        if (data.search_steps && data.search_steps.length > 0) {
            data.search_steps.forEach(step => {
                const tr = document.createElement("tr");
                tr.innerHTML = `
                    <td><strong>Step ${step.step}</strong></td>
                    <td><code>${step.low}</code></td>
                    <td><strong style="color:var(--purple-accent);">${step.mid}</strong></td>
                    <td><code>${step.high}</code></td>
                    <td><strong>#${step.mid_id}</strong></td>
                    <td>${step.action}</td>
                `;
                tableBody.appendChild(tr);
            });
        }
    })
    .catch(err => {
        loading.style.display = "none";
        alert("Error executing binary search in C: " + err);
    });
}

/* ---------------- Chart.js Dashboard Visualizations ---------------- */
function initDashboardCharts(stats) {
    // 1. Progress Doughnut Chart
    const progressCanvas = document.getElementById("progressChart");
    if (progressCanvas) {
        const completed = stats.completed || 0;
        const pending = stats.pending || 0;
        const total = completed + pending;

        new Chart(progressCanvas, {
            type: "doughnut",
            data: {
                labels: ["Completed", "Pending"],
                datasets: [{
                    data: total === 0 ? [0, 1] : [completed, pending],
                    backgroundColor: ["#10b981", "#3b82f6"],
                    borderColor: ["#ffffff", "#ffffff"],
                    borderWidth: 2,
                    hoverOffset: 4
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: { display: false },
                    tooltip: {
                        callbacks: {
                            label: function (context) {
                                const val = context.raw || 0;
                                const pct = total > 0 ? Math.round((val / total) * 100) : 0;
                                return ` ${context.label}: ${val} tasks (${pct}%)`;
                            }
                        }
                    }
                },
                cutout: "70%"
            }
        });
    }

    // 2. Priority Bar Chart
    const priorityCanvas = document.getElementById("priorityChart");
    if (priorityCanvas) {
        new Chart(priorityCanvas, {
            type: "bar",
            data: {
                labels: ["High", "Medium", "Low"],
                datasets: [{
                    label: "Tasks",
                    data: [stats.high || 0, stats.medium || 0, stats.low || 0],
                    backgroundColor: [
                        "rgba(225, 29, 72, 0.85)",
                        "rgba(245, 158, 11, 0.85)",
                        "rgba(16, 185, 129, 0.85)"
                    ],
                    borderColor: ["#e11d48", "#f59e0b", "#10b981"],
                    borderWidth: 1,
                    borderRadius: 6
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: { display: false }
                },
                scales: {
                    y: {
                        beginAtZero: true,
                        ticks: { stepSize: 1, color: "#64748b" },
                        grid: { color: "#f1f5f9" }
                    },
                    x: {
                        ticks: { color: "#64748b" },
                        grid: { display: false }
                    }
                }
            }
        });
    }
}

/* ---------------- Dynamic Calendar Rendering ---------------- */
function renderMiniCalendar(deadlines) {
    const calendarEl = document.getElementById("miniCalendar");
    if (!calendarEl) return;

    const now = new Date();
    const year = now.getFullYear();
    const month = now.getMonth(); // 0-indexed

    const daysInMonth = new Date(year, month + 1, 0).getDate();
    const firstDayIndex = new Date(year, month, 1).getDay(); // 0 is Sun

    // Create deadline lookup map by YYYY-MM-DD
    const deadlineMap = {};
    if (deadlines && Array.isArray(deadlines)) {
        deadlines.forEach(d => {
            if (d.date) {
                if (!deadlineMap[d.date]) deadlineMap[d.date] = [];
                deadlineMap[d.date].push(d);
            }
        });
    }

    let html = `
        <div class="calendar-grid-header">
            <div>S</div><div>M</div><div>T</div><div>W</div><div>T</div><div>F</div><div>S</div>
        </div>
        <div class="calendar-grid-days">
    `;

    // Blank cells before first day of month
    for (let i = 0; i < firstDayIndex; i++) {
        html += `<div class="cal-day empty"></div>`;
    }

    // Days of month
    const todayDate = now.getDate();
    for (let day = 1; day <= daysInMonth; day++) {
        const monthStr = String(month + 1).padStart(2, '0');
        const dayStr = String(day).padStart(2, '0');
        const dateKey = `${year}-${monthStr}-${dayStr}`;

        const isToday = (day === todayDate);
        const dayTasks = deadlineMap[dateKey] || [];
        const hasDeadline = dayTasks.length > 0;

        let dotHtml = '';
        let tooltipText = '';

        if (hasDeadline) {
            const hasHigh = dayTasks.some(t => t.priority === 'High');
            const hasMed = dayTasks.some(t => t.priority === 'Medium');
            const dotClass = hasHigh ? 'high' : (hasMed ? 'med' : 'low');
            dotHtml = `<span class="cal-day-dot ${dotClass}"></span>`;
            tooltipText = dayTasks.map(t => `${t.title} [${t.priority}]`).join("\n");
        }

        html += `
            <div class="cal-day ${isToday ? 'today' : ''} ${hasDeadline ? 'has-deadline' : ''}" 
                 title="${tooltipText ? tooltipText : dateKey}">
                <span>${day}</span>
                ${dotHtml}
            </div>
        `;
    }

    html += `</div>`;
    calendarEl.innerHTML = html;
}

