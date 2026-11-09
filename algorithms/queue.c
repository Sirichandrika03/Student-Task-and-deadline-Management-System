/**
 * ============================================================================
 * STUDENT TASK AND DEADLINE MANAGEMENT SYSTEM
 * Advanced Data Structures (ADS) - PBL Project
 * 
 * Algorithm 2: Circular Queue for FIFO Task Processing
 * File: algorithms/queue.c
 * 
 * Concepts Demonstrated:
 *   - First-In, First-Out (FIFO) discipline
 *   - Front and Rear index arithmetic with modulo wrapping
 *   - Enqueue, Dequeue, Peek, Display
 * Time Complexities:
 *   - Enqueue: O(1)
 *   - Dequeue: O(1)
 *   - Peek:    O(1)
 *   - Display: O(n)
 * Space Complexity: O(CAPACITY)
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUEUE 100
#define MAX_STR 100

typedef struct {
    int task_id;
    char title[MAX_STR];
    char priority[20];
    char deadline[30];
} Task;

typedef struct {
    Task items[MAX_QUEUE];
    int front;
    int rear;
    int size;
} TaskQueue;

void initQueue(TaskQueue* q) {
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

int isFull(TaskQueue* q) {
    return q->size == MAX_QUEUE;
}

int isEmpty(TaskQueue* q) {
    return q->size == 0;
}

int enqueue(TaskQueue* q, int id, const char* title, const char* priority, const char* deadline) {
    if (isFull(q)) {
        return 0; // Queue overflow
    }
    q->rear = (q->rear + 1) % MAX_QUEUE;
    q->items[q->rear].task_id = id;
    strncpy(q->items[q->rear].title, title, MAX_STR - 1);
    q->items[q->rear].title[MAX_STR - 1] = '\0';
    strncpy(q->items[q->rear].priority, priority, 19);
    q->items[q->rear].priority[19] = '\0';
    strncpy(q->items[q->rear].deadline, deadline, 29);
    q->items[q->rear].deadline[29] = '\0';
    q->size++;
    return 1;
}

int dequeue(TaskQueue* q, Task* removed) {
    if (isEmpty(q)) {
        return 0; // Queue underflow
    }
    if (removed != NULL) {
        *removed = q->items[q->front];
    }
    q->front = (q->front + 1) % MAX_QUEUE;
    q->size--;
    return 1;
}

int peek(TaskQueue* q, Task* top) {
    if (isEmpty(q)) return 0;
    if (top != NULL) *top = q->items[q->front];
    return 1;
}

void printJSON(TaskQueue* q, const char* action, const char* message, Task* dequeuedTask) {
    printf("{\n");
    printf("  \"algorithm\": \"FIFO Queue\",\n");
    printf("  \"action\": \"%s\",\n", action);
    printf("  \"status\": \"success\",\n");
    printf("  \"message\": \"%s\",\n", message);
    printf("  \"front_index\": %d,\n", q->front);
    printf("  \"rear_index\": %d,\n", q->rear);
    printf("  \"size\": %d,\n", q->size);
    printf("  \"capacity\": %d,\n", MAX_QUEUE);
    printf("  \"time_complexity\": {\n");
    printf("    \"enqueue\": \"O(1)\",\n");
    printf("    \"dequeue\": \"O(1)\",\n");
    printf("    \"peek\": \"O(1)\",\n");
    printf("    \"display\": \"O(n)\"\n");
    printf("  },\n");

    if (dequeuedTask) {
        printf("  \"dequeued_task\": {\"task_id\": %d, \"title\": \"%s\", \"priority\": \"%s\", \"deadline\": \"%s\"},\n",
               dequeuedTask->task_id, dequeuedTask->title, dequeuedTask->priority, dequeuedTask->deadline);
    } else {
        printf("  \"dequeued_task\": null,\n");
    }

    printf("  \"tasks\": [\n");
    for (int i = 0; i < q->size; i++) {
        int idx = (q->front + i) % MAX_QUEUE;
        printf("    {\"position\": %d, \"task_id\": %d, \"title\": \"%s\", \"priority\": \"%s\", \"deadline\": \"%s\"}%s\n",
               i + 1, q->items[idx].task_id, q->items[idx].title, q->items[idx].priority, q->items[idx].deadline,
               (i < q->size - 1) ? "," : "");
    }
    printf("  ]\n");
    printf("}\n");
}

void runInteractive() {
    TaskQueue q;
    initQueue(&q);
    int choice, id;
    char title[MAX_STR], priority[20], deadline[30];
    Task dequeued;

    printf("\n=======================================================\n");
    printf("       ADS PBL: FIFO TASK QUEUE (C IMPLEMENTATION)      \n");
    printf("=======================================================\n");

    while (1) {
        printf("\n1. Enqueue Task\n2. Dequeue Task (Process Next)\n3. Peek Front Task\n4. Display Queue\n5. Exit\nEnter choice: ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1:
                printf("Enter Task ID: ");
                scanf("%d", &id);
                printf("Enter Title: ");
                scanf(" %[^\n]", title);
                printf("Enter Priority: ");
                scanf(" %s", priority);
                printf("Enter Deadline: ");
                scanf(" %s", deadline);
                if (enqueue(&q, id, title, priority, deadline)) {
                    printf(">> Task [%d: %s] enqueued at rear index %d. Current size: %d\n", id, title, q.rear, q.size);
                } else {
                    printf(">> Queue Overflow! Max capacity reached.\n");
                }
                break;
            case 2:
                if (dequeue(&q, &dequeued)) {
                    printf(">> Task Dequeued (FIFO Order): [ID: %d | Title: '%s' | Priority: %s]\n",
                           dequeued.task_id, dequeued.title, dequeued.priority);
                    printf("   New front index: %d, Remaining tasks: %d\n", q.front, q.size);
                } else {
                    printf(">> Queue Underflow! No tasks to process.\n");
                }
                break;
            case 3:
                if (peek(&q, &dequeued)) {
                    printf(">> Next Task in Queue: [ID: %d | Title: '%s' | Deadline: %s]\n",
                           dequeued.task_id, dequeued.title, dequeued.deadline);
                } else {
                    printf(">> Queue is empty.\n");
                }
                break;
            case 4:
                printf("\n--- CURRENT TASK QUEUE (Front: %d, Rear: %d, Size: %d) ---\n", q.front, q.rear, q.size);
                for (int i = 0; i < q.size; i++) {
                    int idx = (q.front + i) % MAX_QUEUE;
                    printf("  [Pos %d | Slot %d] -> Task ID: %d | Title: '%s' | Priority: %s | Due: %s\n",
                           i + 1, idx, q.items[idx].task_id, q.items[idx].title, q.items[idx].priority, q.items[idx].deadline);
                }
                if (q.size == 0) printf("  (Queue is currently empty)\n");
                break;
            case 5:
                printf("Exiting FIFO Queue demo.\n");
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

    TaskQueue q;
    initQueue(&q);

    if (argc >= 3 && strcmp(argv[1], "batch") == 0) {
        /* Format: id|title|priority|deadline;... */
        char* token = strtok(argv[2], ";");
        while (token != NULL) {
            int id = 0;
            char title[MAX_STR] = {0};
            char priority[20] = {0};
            char deadline[30] = {0};
            char status[20] = {0};
            sscanf(token, "%d|%99[^|]|%19[^|]|%29[^|]|%19s", &id, title, priority, deadline, status);
            if (id > 0) {
                enqueue(&q, id, title, priority, deadline);
            }
            token = strtok(NULL, ";");
        }

        if (argc >= 4 && strcmp(argv[3], "dequeue") == 0) {
            Task removed;
            if (dequeue(&q, &removed)) {
                printJSON(&q, "dequeue", "Task dequeued from front of FIFO queue", &removed);
            } else {
                printJSON(&q, "dequeue", "Queue underflow: queue is empty", NULL);
            }
        } else {
            printJSON(&q, "batch_enqueue", "Tasks enqueued in FIFO order", NULL);
        }
    } else {
        /* Default demo */
        enqueue(&q, 101, "Implement Red-Black Tree in C", "High", "2026-11-10");
        enqueue(&q, 103, "Design Normalized ER Diagram", "High", "2026-11-11");
        enqueue(&q, 102, "Merge Sort Benchmark", "Medium", "2026-11-12");
        printJSON(&q, "demo", "Demo FIFO queue initialized", NULL);
    }

    return 0;
}
