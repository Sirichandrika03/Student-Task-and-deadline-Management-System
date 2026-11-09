/**
 * ============================================================================
 * STUDENT TASK AND DEADLINE MANAGEMENT SYSTEM
 * Advanced Data Structures (ADS) - PBL Project
 * 
 * Algorithm 3: Stack for Recently Completed Tasks & Undo History
 * File: algorithms/stack.c
 * 
 * Concepts Demonstrated:
 *   - Last-In, First-Out (LIFO) discipline
 *   - Push (task completion), Pop (undo completion), Peek (latest completed)
 * Time Complexities:
 *   - Push: O(1)
 *   - Pop:  O(1)
 *   - Peek: O(1)
 *   - Display: O(n)
 * Space Complexity: O(CAPACITY)
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK 100
#define MAX_TITLE 100

typedef struct {
    int task_id;
    char title[MAX_TITLE];
    char completed_at[30];
} StackItem;

typedef struct {
    StackItem data[MAX_STACK];
    int top;
} TaskStack;

void initStack(TaskStack* s) {
    s->top = -1;
}

int isFull(TaskStack* s) {
    return s->top == MAX_STACK - 1;
}

int isEmpty(TaskStack* s) {
    return s->top == -1;
}

int push(TaskStack* s, int id, const char* title, const char* completed_at) {
    if (isFull(s)) return 0; // Stack Overflow
    s->top++;
    s->data[s->top].task_id = id;
    strncpy(s->data[s->top].title, title, MAX_TITLE - 1);
    s->data[s->top].title[MAX_TITLE - 1] = '\0';
    strncpy(s->data[s->top].completed_at, completed_at ? completed_at : "Just now", 29);
    s->data[s->top].completed_at[29] = '\0';
    return 1;
}

int pop(TaskStack* s, StackItem* popped) {
    if (isEmpty(s)) return 0; // Stack Underflow
    if (popped != NULL) {
        *popped = s->data[s->top];
    }
    s->top--;
    return 1;
}

int peek(TaskStack* s, StackItem* topItem) {
    if (isEmpty(s)) return 0;
    if (topItem != NULL) {
        *topItem = s->data[s->top];
    }
    return 1;
}

void printJSON(TaskStack* s, const char* action, const char* message, StackItem* poppedItem) {
    printf("{\n");
    printf("  \"algorithm\": \"LIFO Stack (Completed Tasks / Undo History)\",\n");
    printf("  \"action\": \"%s\",\n", action);
    printf("  \"status\": \"success\",\n");
    printf("  \"message\": \"%s\",\n", message);
    printf("  \"top_index\": %d,\n", s->top);
    printf("  \"total_items\": %d,\n", s->top + 1);
    printf("  \"time_complexity\": {\n");
    printf("    \"push\": \"O(1)\",\n");
    printf("    \"pop\": \"O(1)\",\n");
    printf("    \"peek\": \"O(1)\",\n");
    printf("    \"display\": \"O(n)\"\n");
    printf("  },\n");

    if (poppedItem) {
        printf("  \"popped_task\": {\"task_id\": %d, \"title\": \"%s\", \"completed_at\": \"%s\"},\n",
               poppedItem->task_id, poppedItem->title, poppedItem->completed_at);
    } else {
        printf("  \"popped_task\": null,\n");
    }

    printf("  \"stack_history\": [\n");
    /* Print from top of stack downward to show most recently completed first */
    for (int i = s->top; i >= 0; i--) {
        printf("    {\"stack_level\": %d, \"task_id\": %d, \"title\": \"%s\", \"completed_at\": \"%s\"}%s\n",
               i, s->data[i].task_id, s->data[i].title, s->data[i].completed_at, (i > 0) ? "," : "");
    }
    printf("  ]\n");
    printf("}\n");
}

void runInteractive() {
    TaskStack s;
    initStack(&s);
    int choice, id;
    char title[MAX_TITLE], timeStr[30];
    StackItem item;

    printf("\n=======================================================\n");
    printf("      ADS PBL: LIFO TASK STACK (C IMPLEMENTATION)      \n");
    printf("=======================================================\n");

    while (1) {
        printf("\n1. Push Completed Task\n2. Pop Task (Undo Complete)\n3. Peek Most Recent Completed\n4. Display Stack\n5. Exit\nEnter choice: ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1:
                printf("Enter Task ID: ");
                scanf("%d", &id);
                printf("Enter Title: ");
                scanf(" %[^\n]", title);
                printf("Enter Completion Timestamp (or 'now'): ");
                scanf(" %s", timeStr);
                if (push(&s, id, title, timeStr)) {
                    printf(">> Pushed task [%d: '%s'] to top of stack (Index %d).\n", id, title, s.top);
                } else {
                    printf(">> Stack Overflow!\n");
                }
                break;
            case 2:
                if (pop(&s, &item)) {
                    printf(">> Popped (Undo) Task: [ID: %d | Title: '%s' | Completed: %s]\n",
                           item.task_id, item.title, item.completed_at);
                    printf("   New top index: %d\n", s.top);
                } else {
                    printf(">> Stack Underflow! Stack is empty.\n");
                }
                break;
            case 3:
                if (peek(&s, &item)) {
                    printf(">> Top (Most Recent) Task: [ID: %d | Title: '%s' | Time: %s]\n",
                           item.task_id, item.title, item.completed_at);
                } else {
                    printf(">> Stack is empty.\n");
                }
                break;
            case 4:
                printf("\n--- RECENTLY COMPLETED TASK STACK (Top: %d, Count: %d) ---\n", s.top, s.top + 1);
                for (int i = s.top; i >= 0; i--) {
                    printf("  [LIFO Level %d %s] -> Task ID: %d | '%s' | Completed: %s\n",
                           i, (i == s.top ? "<-- TOP" : "       "), s.data[i].task_id, s.data[i].title, s.data[i].completed_at);
                }
                if (s.top == -1) printf("  (Stack is currently empty)\n");
                break;
            case 5:
                printf("Exiting Stack demo.\n");
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

    TaskStack s;
    initStack(&s);

    if (argc >= 3 && strcmp(argv[1], "batch") == 0) {
        /* Format: id|title|time;... */
        char* token = strtok(argv[2], ";");
        while (token != NULL) {
            int id = 0;
            char title[MAX_TITLE] = {0};
            char timeStr[30] = {0};
            sscanf(token, "%d|%99[^|]|%29s", &id, title, timeStr);
            if (id > 0) {
                push(&s, id, title, timeStr);
            }
            token = strtok(NULL, ";");
        }

        if (argc >= 4 && strcmp(argv[3], "pop") == 0) {
            StackItem popped;
            if (pop(&s, &popped)) {
                printJSON(&s, "pop", "Top completed task popped (Undo action)", &popped);
            } else {
                printJSON(&s, "pop", "Stack underflow: no tasks to pop", NULL);
            }
        } else {
            printJSON(&s, "batch_push", "Tasks pushed to completion stack", NULL);
        }
    } else {
        /* Default demo */
        push(&s, 107, "Binary Search on Sorted Task IDs", "2026-11-07 14:30:00");
        push(&s, 108, "SQL Complex Join Queries Lab", "2026-11-08 17:15:00");
        printJSON(&s, "demo", "Demo task completion stack initialized", NULL);
    }

    return 0;
}
