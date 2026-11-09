/**
 * ============================================================================
 * STUDENT TASK AND DEADLINE MANAGEMENT SYSTEM
 * Advanced Data Structures (ADS) - PBL Project
 * 
 * Algorithm 6: Binary Search on Sorted Task IDs
 * File: algorithms/binary_search.c
 * 
 * Concepts Demonstrated:
 *   - Precondition: Array must be sorted by key (task_id)
 *   - Divide search space in half at each iteration
 *   - Iteration trace: low, mid, high, comparisons
 * Time Complexities:
 *   - Best Case:    O(1) [Target at initial midpoint]
 *   - Average Case: O(log n)
 *   - Worst Case:   O(log n)
 * Space Complexity: O(1) iterative
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
    char deadline[30];
    char status[20];
} Task;

/* Trace structure to record step-by-step iterations */
typedef struct {
    int step;
    int low;
    int mid;
    int high;
    int mid_val;
    char comparison[30];
} SearchStep;

/* Sort tasks by task_id using insertion sort to ensure precondition */
void sortById(Task arr[], int n) {
    for (int i = 1; i < n; i++) {
        Task key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].task_id > key.task_id) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

/* Binary Search with detailed iteration trace recording */
int binarySearch(Task arr[], int n, int target_id, SearchStep steps[], int* step_count) {
    int low = 0;
    int high = n - 1;
    *step_count = 0;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        SearchStep* s = &steps[*step_count];
        s->step = *step_count + 1;
        s->low = low;
        s->mid = mid;
        s->high = high;
        s->mid_val = arr[mid].task_id;

        if (arr[mid].task_id == target_id) {
            snprintf(s->comparison, sizeof(s->comparison), "%d == %d (Found!)", arr[mid].task_id, target_id);
            (*step_count)++;
            return mid; // Target found
        } else if (arr[mid].task_id < target_id) {
            snprintf(s->comparison, sizeof(s->comparison), "%d < %d (Search Right)", arr[mid].task_id, target_id);
            (*step_count)++;
            low = mid + 1;
        } else {
            snprintf(s->comparison, sizeof(s->comparison), "%d > %d (Search Left)", arr[mid].task_id, target_id);
            (*step_count)++;
            high = mid - 1;
        }
    }

    return -1; // Target not found
}

void printJSON(Task arr[], int n, int target_id, int found_index, SearchStep steps[], int step_count) {
    printf("{\n");
    printf("  \"algorithm\": \"Binary Search\",\n");
    printf("  \"target_id\": %d,\n", target_id);
    printf("  \"found\": %s,\n", (found_index != -1) ? "true" : "false");
    printf("  \"total_tasks\": %d,\n", n);
    printf("  \"comparison_count\": %d,\n", step_count);
    printf("  \"time_complexity\": {\n");
    printf("    \"best\": \"O(1)\",\n");
    printf("    \"average\": \"O(log n)\",\n");
    printf("    \"worst\": \"O(log n)\"\n");
    printf("  },\n");
    printf("  \"space_complexity\": \"O(1)\",\n");

    if (found_index != -1) {
        printf("  \"found_task\": {\"task_id\": %d, \"title\": \"%s\", \"priority\": \"%s\", \"deadline\": \"%s\", \"status\": \"%s\", \"index\": %d},\n",
               arr[found_index].task_id, arr[found_index].title, arr[found_index].priority, arr[found_index].deadline, arr[found_index].status, found_index);
    } else {
        printf("  \"found_task\": null,\n");
    }

    printf("  \"search_steps\": [\n");
    for (int i = 0; i < step_count; i++) {
        printf("    {\"step\": %d, \"low\": %d, \"mid\": %d, \"high\": %d, \"mid_id\": %d, \"action\": \"%s\"}%s\n",
               steps[i].step, steps[i].low, steps[i].mid, steps[i].high, steps[i].mid_val, steps[i].comparison,
               (i < step_count - 1) ? "," : "");
    }
    printf("  ],\n");

    printf("  \"sorted_id_list\": [");
    for (int i = 0; i < n; i++) {
        printf("%d%s", arr[i].task_id, (i < n - 1) ? ", " : "");
    }
    printf("]\n");
    printf("}\n");
}

void runInteractive() {
    int n, target;
    Task tasks[MAX_TASKS];
    SearchStep steps[30];
    int step_count = 0;

    printf("\n=======================================================\n");
    printf("    ADS PBL: TASK BINARY SEARCH (C IMPLEMENTATION)    \n");
    printf("=======================================================\n");
    printf("Enter number of tasks: ");
    if (scanf("%d", &n) != 1 || n <= 0 || n > MAX_TASKS) return;

    for (int i = 0; i < n; i++) {
        printf("\nTask %d:\n", i + 1);
        printf("  Task ID: ");
        scanf("%d", &tasks[i].task_id);
        printf("  Title: ");
        scanf(" %[^\n]", tasks[i].title);
        printf("  Priority: ");
        scanf(" %s", tasks[i].priority);
        printf("  Deadline: ");
        scanf(" %s", tasks[i].deadline);
        strcpy(tasks[i].status, "Pending");
    }

    sortById(tasks, n);
    printf("\n--- PRE-CONDITION: TASKS SORTED BY TASK ID ---\n");
    for (int i = 0; i < n; i++) {
        printf("  Index [%d] -> Task ID: %d | Title: '%s'\n", i, tasks[i].task_id, tasks[i].title);
    }

    while (1) {
        printf("\nEnter Task ID to search (-1 to exit): ");
        if (scanf("%d", &target) != 1 || target == -1) break;

        int res = binarySearch(tasks, n, target, steps, &step_count);

        printf("\n--- BINARY SEARCH TRACE LOG ---\n");
        for (int i = 0; i < step_count; i++) {
            printf("  Step %d: Range [%d..%d] -> Mid: %d (Task ID %d) -> Result: %s\n",
                   steps[i].step, steps[i].low, steps[i].high, steps[i].mid, steps[i].mid_val, steps[i].comparison);
        }

        if (res != -1) {
            printf("\n>> SUCCESS: Task found at sorted array index %d!\n", res);
            printf("   Task Details: ID=%d, Title='%s', Priority=%s, Due=%s\n",
                   tasks[res].task_id, tasks[res].title, tasks[res].priority, tasks[res].deadline);
            printf("   Search took %d comparisons. O(log %d) <= %d\n", step_count, n, step_count);
        } else {
            printf("\n>> NOT FOUND: Task ID %d does not exist in the collection.\n", target);
            printf("   Search concluded after %d comparisons.\n", step_count);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--interactive") == 0) {
        runInteractive();
        return 0;
    }

    Task tasks[MAX_TASKS];
    int count = 0;
    int target_id = 103;

    if (argc >= 4 && strcmp(argv[1], "search") == 0) {
        target_id = atoi(argv[2]);
        /* argv[3] is batch data: id|title|priority|deadline|status;... */
        char* token = strtok(argv[3], ";");
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
                strncpy(tasks[count].status, status, 19);
                count++;
            }
            token = strtok(NULL, ";");
        }
    } else {
        /* Default demo */
        tasks[0] = (Task){101, "Red-Black Tree in C", "High", "2026-11-10", "Pending"};
        tasks[1] = (Task){102, "Merge Sort Benchmark", "Medium", "2026-11-12", "Pending"};
        tasks[2] = (Task){103, "Normalized ER Diagram", "High", "2026-11-11", "Pending"};
        tasks[3] = (Task){104, "Dining Philosophers Lab", "Medium", "2026-11-14", "Pending"};
        tasks[4] = (Task){105, "TCP vs UDP Socket Lab", "Low", "2026-11-16", "Pending"};
        count = 5;
    }

    /* Ensure tasks are sorted by task_id */
    sortById(tasks, count);

    SearchStep steps[30];
    int step_count = 0;
    int idx = binarySearch(tasks, count, target_id, steps, &step_count);

    printJSON(tasks, count, target_id, idx, steps, step_count);

    return 0;
}

