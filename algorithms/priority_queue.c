/**
 * ============================================================================
 * STUDENT TASK AND DEADLINE MANAGEMENT SYSTEM
 * Advanced Data Structures (ADS) - PBL Project
 * 
 * Algorithm 4: Binary Max-Heap Priority Queue for Task Scheduling
 * File: algorithms/priority_queue.c
 * 
 * Priority Rules:
 *   1. Primary Order:
 *        High   = 3
 *        Medium = 2
 *        Low    = 1
 *   2. Secondary Tie-Breaker:
 *        If two tasks have equal priority, compare their deadlines.
 *        The task with the nearest (earliest) deadline appears first.
 *   3. Tertiary Tie-Breaker:
 *        Smaller Task ID appears first for deterministic ordering.
 * 
 * Concepts Demonstrated:
 *   - Binary Max-Heap Tree representation in 1D Array
 *   - Heap property: parent key >= children keys with composite comparison
 *   - Sift-Up (Heapify-Up) on Insertion: O(log n)
 *   - Sift-Down (Heapify-Down) on Extraction: O(log n)
 *   - Extract-Max (Highest priority first): O(log n)
 * 
 * Time Complexities:
 *   - Insertion:    O(log n)
 *   - Extract Max:  O(log n)
 *   - Peek Max:     O(1)
 *   - Build Heap:   O(n)
 *   - HeapSort:     O(n log n)
 * Space Complexity: O(CAPACITY)
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEAP 100
#define MAX_TITLE 100

typedef struct {
    int task_id;
    char title[MAX_TITLE];
    char priority_str[20];
    int priority_score; // High=3, Medium=2, Low=1
    char deadline[30];
} HeapNode;

typedef struct {
    HeapNode tree[MAX_HEAP];
    int size;
} PriorityQueue;

/* Convert string priority to numeric score: High=3, Medium=2, Low=1 */
int getPriorityScore(const char* priority) {
    if (strcasecmp(priority, "High") == 0) return 3;
    if (strcasecmp(priority, "Medium") == 0) return 2;
    if (strcasecmp(priority, "Low") == 0) return 1;
    return 1;
}

void initPQ(PriorityQueue* pq) {
    pq->size = 0;
}

void swapNodes(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Compare two tasks in the Max-Heap:
 * Returns 1 if task 'a' has strictly higher priority than task 'b', 0 otherwise.
 * 
 * Criteria:
 *   1. Higher priority score comes first (High=3 > Medium=2 > Low=1)
 *   2. If priority scores are equal, the task with the NEAREST (earliest) deadline comes first.
 *   3. If deadlines are also equal, smaller task_id comes first for determinism.
 */
int hasHigherPriority(const HeapNode* a, const HeapNode* b) {
    // Primary criterion: Priority Score
    if (a->priority_score > b->priority_score) return 1;
    if (a->priority_score < b->priority_score) return 0;

    // Secondary tie-breaker: Nearest deadline first (chronological ISO string comparison)
    if (strlen(a->deadline) > 0 && strlen(b->deadline) > 0) {
        int cmp = strcmp(a->deadline, b->deadline);
        if (cmp < 0) return 1; // 'a' deadline is earlier/nearer than 'b'
        if (cmp > 0) return 0; // 'b' deadline is earlier/nearer than 'a'
    }

    // Tertiary tie-breaker: smaller task_id
    return a->task_id < b->task_id ? 1 : 0;
}

/* Sift-up element at index to restore Max-Heap property */
void siftUp(PriorityQueue* pq, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (hasHigherPriority(&pq->tree[index], &pq->tree[parent])) {
            swapNodes(&pq->tree[index], &pq->tree[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

/* Sift-down element at index to restore Max-Heap property */
void siftDown(PriorityQueue* pq, int index) {
    while (1) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int highest = index;

        if (left < pq->size && hasHigherPriority(&pq->tree[left], &pq->tree[highest])) {
            highest = left;
        }
        if (right < pq->size && hasHigherPriority(&pq->tree[right], &pq->tree[highest])) {
            highest = right;
        }

        if (highest != index) {
            swapNodes(&pq->tree[index], &pq->tree[highest]);
            index = highest;
        } else {
            break;
        }
    }
}

/* Insert new task into Max-Heap Priority Queue */
int insertPQ(PriorityQueue* pq, int id, const char* title, const char* priority, const char* deadline) {
    if (pq->size >= MAX_HEAP) return 0; // Heap Full

    int idx = pq->size;
    pq->tree[idx].task_id = id;
    strncpy(pq->tree[idx].title, title, MAX_TITLE - 1);
    pq->tree[idx].title[MAX_TITLE - 1] = '\0';
    strncpy(pq->tree[idx].priority_str, priority, 19);
    pq->tree[idx].priority_str[19] = '\0';
    pq->tree[idx].priority_score = getPriorityScore(priority);
    strncpy(pq->tree[idx].deadline, deadline, 29);
    pq->tree[idx].deadline[29] = '\0';

    pq->size++;
    siftUp(pq, idx);
    return 1;
}

/* Extract highest priority task (Root of Heap) */
int extractMax(PriorityQueue* pq, HeapNode* maxItem) {
    if (pq->size <= 0) return 0;

    if (maxItem != NULL) {
        *maxItem = pq->tree[0];
    }

    pq->tree[0] = pq->tree[pq->size - 1];
    pq->size--;
    if (pq->size > 0) {
        siftDown(pq, 0);
    }
    return 1;
}

/* Peek at root */
int peekMax(PriorityQueue* pq, HeapNode* maxItem) {
    if (pq->size <= 0) return 0;
    if (maxItem != NULL) {
        *maxItem = pq->tree[0];
    }
    return 1;
}

void printJSON(PriorityQueue* pq, const char* action, const char* message, HeapNode* extracted) {
    /* Copy heap to perform non-destructive HeapSort for display */
    PriorityQueue copy = *pq;
    HeapNode sortedList[MAX_HEAP];
    int count = copy.size;
    for (int i = 0; i < count; i++) {
        extractMax(&copy, &sortedList[i]);
    }

    printf("{\n");
    printf("  \"algorithm\": \"Priority Queue (Binary Max-Heap with Deadline Tie-Breaker)\",\n");
    printf("  \"action\": \"%s\",\n", action);
    printf("  \"status\": \"success\",\n");
    printf("  \"message\": \"%s\",\n", message);
    printf("  \"priority_rules\": {\n");
    printf("    \"primary\": \"High = 3, Medium = 2, Low = 1\",\n");
    printf("    \"secondary\": \"Equal priority -> nearest deadline first\"\n");
    printf("  },\n");
    printf("  \"heap_size\": %d,\n", pq->size);
    printf("  \"time_complexity\": {\n");
    printf("    \"insert\": \"O(log n)\",\n");
    printf("    \"extract_max\": \"O(log n)\",\n");
    printf("    \"peek\": \"O(1)\",\n");
    printf("    \"heap_sort\": \"O(n log n)\"\n");
    printf("  },\n");

    if (extracted) {
        printf("  \"extracted_task\": {\"task_id\": %d, \"title\": \"%s\", \"priority\": \"%s\", \"score\": %d, \"deadline\": \"%s\"},\n",
               extracted->task_id, extracted->title, extracted->priority_str, extracted->priority_score, extracted->deadline);
    } else {
        printf("  \"extracted_task\": null,\n");
    }

    /* Heap Array internal representation */
    printf("  \"heap_array\": [\n");
    for (int i = 0; i < pq->size; i++) {
        printf("    {\"index\": %d, \"task_id\": %d, \"priority\": \"%s\", \"score\": %d, \"deadline\": \"%s\", \"title\": \"%s\"}%s\n",
               i, pq->tree[i].task_id, pq->tree[i].priority_str, pq->tree[i].priority_score, pq->tree[i].deadline, pq->tree[i].title,
               (i < pq->size - 1) ? "," : "");
    }
    printf("  ],\n");

    /* Sorted output by priority (High -> Med -> Low, nearest deadline first) */
    printf("  \"priority_ordered_tasks\": [\n");
    for (int i = 0; i < count; i++) {
        printf("    {\"rank\": %d, \"task_id\": %d, \"title\": \"%s\", \"priority\": \"%s\", \"score\": %d, \"deadline\": \"%s\"}%s\n",
               i + 1, sortedList[i].task_id, sortedList[i].title, sortedList[i].priority_str, sortedList[i].priority_score, sortedList[i].deadline,
               (i < count - 1) ? "," : "");
    }
    printf("  ]\n");
    printf("}\n");
}

void runInteractive() {
    PriorityQueue pq;
    initPQ(&pq);
    int choice, id;
    char title[MAX_TITLE], priority[20], deadline[30];
    HeapNode node;

    printf("\n=======================================================\n");
    printf("   ADS PBL: MAX-HEAP PRIORITY QUEUE (C IMPLEMENTATION) \n");
    printf("   Rules: High=3, Medium=2, Low=1                      \n");
    printf("   Tie-Breaker: Nearest Deadline First                 \n");
    printf("=======================================================\n");

    while (1) {
        printf("\n1. Insert Task with Priority & Deadline\n2. Extract Max Priority Task\n3. Peek Highest Priority Task\n4. Display Heap & Priority Schedule\n5. Exit\nEnter choice: ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1:
                printf("Enter Task ID: ");
                scanf("%d", &id);
                printf("Enter Title: ");
                scanf(" %[^\n]", title);
                printf("Enter Priority (High, Medium, Low): ");
                scanf(" %s", priority);
                printf("Enter Deadline (YYYY-MM-DD): ");
                scanf(" %s", deadline);
                if (insertPQ(&pq, id, title, priority, deadline)) {
                    printf(">> Task %d inserted. Heap size: %d, Root is: Task %d [%s | Due: %s]\n",
                           id, pq.size, pq.tree[0].task_id, pq.tree[0].priority_str, pq.tree[0].deadline);
                } else {
                    printf(">> Heap Overflow!\n");
                }
                break;
            case 2:
                if (extractMax(&pq, &node)) {
                    printf(">> Extracted Highest Priority Task: [ID: %d | '%s' | Priority: %s (Score: %d) | Due: %s]\n",
                           node.task_id, node.title, node.priority_str, node.priority_score, node.deadline);
                    printf("   Remaining heap size: %d\n", pq.size);
                } else {
                    printf(">> Heap Underflow! No tasks available.\n");
                }
                break;
            case 3:
                if (peekMax(&pq, &node)) {
                    printf(">> Root Task (Highest Priority): [ID: %d | '%s' | Priority: %s | Due: %s]\n",
                           node.task_id, node.title, node.priority_str, node.deadline);
                } else {
                    printf(">> Priority Queue is empty.\n");
                }
                break;
            case 4: {
                printf("\n--- CURRENT HEAP ARRAY (Size: %d) ---\n", pq.size);
                for (int i = 0; i < pq.size; i++) {
                    int parent = (i - 1) / 2;
                    printf("  [Index %d] Task %d | %s (Score %d) | Due: %s | Title: '%s' | Parent: %s\n",
                           i, pq.tree[i].task_id, pq.tree[i].priority_str, pq.tree[i].priority_score, pq.tree[i].deadline, pq.tree[i].title,
                           (i == 0) ? "None (ROOT)" : pq.tree[parent].title);
                }
                printf("\n--- PRIORITY SCHEDULE (High->Med->Low, Nearest Deadline First) ---\n");
                PriorityQueue temp = pq;
                int rank = 1;
                while (extractMax(&temp, &node)) {
                    printf("  Rank %d: [Task %d] '%s' -> Priority: %s (Score %d) | Due: %s\n",
                           rank++, node.task_id, node.title, node.priority_str, node.priority_score, node.deadline);
                }
                break;
            }
            case 5:
                printf("Exiting Priority Queue demo.\n");
                return;
            default:
                printf("Invalid choice!\n");
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--interactive") == 0) {
        runInteractive();
        return 0;
    }

    PriorityQueue pq;
    initPQ(&pq);

    if (argc >= 3 && strcmp(argv[1], "batch") == 0) {
        /* Format: id|title|priority|deadline;... */
        char* token = strtok(argv[2], ";");
        while (token != NULL) {
            int id = 0;
            char title[MAX_TITLE] = {0};
            char priority[20] = {0};
            char deadline[30] = {0};
            char status[20] = {0};
            sscanf(token, "%d|%99[^|]|%19[^|]|%29[^|]|%19s", &id, title, priority, deadline, status);
            if (id > 0) {
                insertPQ(&pq, id, title, priority, deadline);
            }
            token = strtok(NULL, ";");
        }

        if (argc >= 4 && strcmp(argv[3], "extract") == 0) {
            HeapNode top;
            if (extractMax(&pq, &top)) {
                printJSON(&pq, "extract_max", "Highest priority task extracted from Max-Heap", &top);
            } else {
                printJSON(&pq, "extract_max", "Heap underflow: priority queue is empty", NULL);
            }
        } else {
            printJSON(&pq, "batch_insert", "Tasks inserted into Max-Heap Priority Queue", NULL);
        }
    } else {
        /* Default demo demonstrating equal priority with nearest deadline tie-breaker */
        insertPQ(&pq, 101, "Red-Black Tree in C", "High", "2026-11-15");
        insertPQ(&pq, 103, "Normalized ER Diagram", "High", "2026-11-10"); // Same High priority, nearer deadline -> Root!
        insertPQ(&pq, 102, "Merge Sort Benchmark", "Medium", "2026-11-18");
        insertPQ(&pq, 104, "Dining Philosophers Lab", "Medium", "2026-11-12"); // Same Medium priority, nearer deadline
        insertPQ(&pq, 105, "TCP Socket Lab", "Low", "2026-11-20");
        printJSON(&pq, "demo", "Demo Max-Heap Priority Queue with deadline tie-breaker initialized", NULL);
    }

    return 0;
}
