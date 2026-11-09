/**
 * ============================================================================
 * STUDENT TASK AND DEADLINE MANAGEMENT SYSTEM
 * Advanced Data Structures (ADS) - PBL Project
 * 
 * Algorithm 5: Merge Sort (Divide & Conquer) for Task Deadlines
 * File: algorithms/merge_sort.c
 * 
 * Concepts Demonstrated:
 *   - Divide and Conquer Paradigm
 *   - Recursive splitting into Left and Right subproblems
 *   - Linear two-pointer Merge subroutine
 *   - Stable O(n log n) sorting guarantee across all cases
 * Time Complexities:
 *   - Best Case:    O(n log n)
 *   - Average Case: O(n log n)
 *   - Worst Case:   O(n log n)
 * Space Complexity: O(n) auxiliary storage
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 200
#define MAX_TITLE 100

typedef struct {
    int task_id;
    char title[MAX_TITLE];
    char priority[20];
    char deadline[30]; // Format: YYYY-MM-DD or YYYY-MM-DD HH:MM:SS
} Task;

/* Global recursion step counter and trace log */
static int recursion_depth = 0;
static int total_merges = 0;
static char trace_log[2048] = "";

/* Compare two deadlines lexicographically (ISO format YYYY-MM-DD ensures correct chronological order) */
int compareDeadlines(const char* d1, const char* d2) {
    return strcmp(d1, d2);
}

/* Merge two sorted subarrays: arr[left..mid] and arr[mid+1..right] */
void merge(Task arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Task* L = (Task*)malloc(n1 * sizeof(Task));
    Task* R = (Task*)malloc(n2 * sizeof(Task));

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (compareDeadlines(L[i].deadline, R[j].deadline) <= 0) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    total_merges++;
    char step[128];
    snprintf(step, sizeof(step), "Merge step %d: sub-arrays [%d..%d] and [%d..%d] combined | ",
             total_merges, left, mid, mid + 1, right);
    if (strlen(trace_log) + strlen(step) < sizeof(trace_log) - 1) {
        strcat(trace_log, step);
    }

    free(L);
    free(R);
}

/* Recursive Merge Sort function */
void mergeSort(Task arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void printJSON(Task arr[], int count, const char* action, const char* message) {
    printf("{\n");
    printf("  \"algorithm\": \"Merge Sort\",\n");
    printf("  \"action\": \"%s\",\n", action);
    printf("  \"status\": \"success\",\n");
    printf("  \"message\": \"%s\",\n", message);
    printf("  \"task_count\": %d,\n", count);
    printf("  \"total_merges\": %d,\n", total_merges);
    printf("  \"trace_summary\": \"%s\",\n", trace_log);
    printf("  \"time_complexity\": {\n");
    printf("    \"best\": \"O(n log n)\",\n");
    printf("    \"average\": \"O(n log n)\",\n");
    printf("    \"worst\": \"O(n log n)\"\n");
    printf("  },\n");
    printf("  \"space_complexity\": \"O(n)\",\n");
    printf("  \"sorted_tasks\": [\n");

    for (int i = 0; i < count; i++) {
        printf("    {\"rank\": %d, \"task_id\": %d, \"title\": \"%s\", \"priority\": \"%s\", \"deadline\": \"%s\"}%s\n",
               i + 1, arr[i].task_id, arr[i].title, arr[i].priority, arr[i].deadline, (i < count - 1) ? "," : "");
    }
    printf("  ]\n");
    printf("}\n");
}

void runInteractive() {
    int n;
    Task tasks[MAX_TASKS];

    printf("\n=======================================================\n");
    printf("    ADS PBL: DEADLINE MERGE SORT (C IMPLEMENTATION)   \n");
    printf("=======================================================\n");
    printf("Enter number of tasks to sort: ");
    if (scanf("%d", &n) != 1 || n <= 0 || n > MAX_TASKS) return;

    for (int i = 0; i < n; i++) {
        printf("\nTask %d of %d:\n", i + 1, n);
        printf("  Task ID: ");
        scanf("%d", &tasks[i].task_id);
        printf("  Title: ");
        scanf(" %[^\n]", tasks[i].title);
        printf("  Priority: ");
        scanf(" %s", tasks[i].priority);
        printf("  Deadline (YYYY-MM-DD): ");
        scanf(" %s", tasks[i].deadline);
    }

    printf("\n--- ORIGINAL UNSORTED TASKS ---\n");
    for (int i = 0; i < n; i++) {
        printf("  [%d] ID: %d | Due: %s | Title: %s\n", i + 1, tasks[i].task_id, tasks[i].deadline, tasks[i].title);
    }

    total_merges = 0;
    trace_log[0] = '\0';
    mergeSort(tasks, 0, n - 1);

    printf("\n--- DEADLINE-SORTED TASKS (Earliest First via Merge Sort) ---\n");
    for (int i = 0; i < n; i++) {
        printf("  [Rank %d] Due: %s | Task ID: %d | Priority: %s | Title: '%s'\n",
               i + 1, tasks[i].deadline, tasks[i].task_id, tasks[i].priority, tasks[i].title);
    }
    printf("\nMerge sort completed with %d merge operations. Time complexity: O(%d log %d)\n", total_merges, n, n);
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--interactive") == 0) {
        runInteractive();
        return 0;
    }

    Task tasks[MAX_TASKS];
    int count = 0;

    if (argc >= 3 && strcmp(argv[1], "batch") == 0) {
        /* Format: id|title|priority|deadline;... */
        char* token = strtok(argv[2], ";");
        while (token != NULL && count < MAX_TASKS) {
            int id = 0;
            char title[MAX_TITLE] = {0};
            char priority[20] = {0};
            char deadline[30] = {0};
            char status[20] = {0};
            sscanf(token, "%d|%99[^|]|%19[^|]|%29[^|]|%19s", &id, title, priority, deadline, status);
            if (id > 0) {
                tasks[count].task_id = id;
                strncpy(tasks[count].title, title, MAX_TITLE - 1);
                strncpy(tasks[count].priority, priority, 19);
                strncpy(tasks[count].deadline, deadline, 29);
                count++;
            }
            token = strtok(NULL, ";");
        }

        if (count > 0) {
            mergeSort(tasks, 0, count - 1);
        }
        printJSON(tasks, count, "sort_by_deadline", "Tasks sorted chronologically by deadline using Merge Sort");
    } else {
        /* Default demo sample */
        tasks[0] = (Task){105, "TCP vs UDP Socket Lab", "Low", "2026-11-16 10:00:00"};
        tasks[1] = (Task){101, "Red-Black Tree in C", "High", "2026-11-10 23:59:00"};
        tasks[2] = (Task){103, "Normalized ER Diagram", "High", "2026-11-11 18:00:00"};
        tasks[3] = (Task){102, "Merge Sort Benchmark", "Medium", "2026-11-12 12:00:00"};
        tasks[4] = (Task){104, "Dining Philosophers Lab", "Medium", "2026-11-14 15:30:00"};
        count = 5;

        mergeSort(tasks, 0, count - 1);
        printJSON(tasks, count, "demo", "Sample tasks sorted by deadline");
    }

    return 0;
}
