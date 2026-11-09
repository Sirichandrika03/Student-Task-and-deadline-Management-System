/**
 * ============================================================================
 * STUDENT TASK AND DEADLINE MANAGEMENT SYSTEM
 * Advanced Data Structures (ADS) - PBL Project
 * 
 * Algorithm 1: Singly Linked List for Dynamic Task Management
 * File: algorithms/linked_list.c
 * 
 * Time Complexities:
 *   - Insertion at Head: O(1)
 *   - Insertion at Tail: O(n) [or O(1) with tail pointer]
 *   - Deletion by ID:    O(n)
 *   - Search / Display:  O(n)
 * Space Complexity:      O(n) dynamic node allocation
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE 100
#define MAX_PRIORITY 15
#define MAX_DATE 25

/* Task Node Structure */
typedef struct TaskNode {
    int task_id;
    char title[MAX_TITLE];
    char priority[MAX_PRIORITY];
    char deadline[MAX_DATE];
    char status[MAX_PRIORITY];
    struct TaskNode* next;
} TaskNode;

/* Linked List Control Structure */
typedef struct {
    TaskNode* head;
    TaskNode* tail;
    int count;
} LinkedList;

/* Initialize empty Linked List */
void initList(LinkedList* list) {
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

/* Create a new TaskNode */
TaskNode* createNode(int id, const char* title, const char* priority, const char* deadline, const char* status) {
    TaskNode* newNode = (TaskNode*)malloc(sizeof(TaskNode));
    if (!newNode) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    newNode->task_id = id;
    strncpy(newNode->title, title, MAX_TITLE - 1);
    newNode->title[MAX_TITLE - 1] = '\0';
    strncpy(newNode->priority, priority, MAX_PRIORITY - 1);
    newNode->priority[MAX_PRIORITY - 1] = '\0';
    strncpy(newNode->deadline, deadline, MAX_DATE - 1);
    newNode->deadline[MAX_DATE - 1] = '\0';
    strncpy(newNode->status, status ? status : "Pending", MAX_PRIORITY - 1);
    newNode->status[MAX_PRIORITY - 1] = '\0';
    newNode->next = NULL;
    return newNode;
}

/* Insert task at the end of the list (Append) - O(1) with tail pointer */
void insertTask(LinkedList* list, int id, const char* title, const char* priority, const char* deadline, const char* status) {
    TaskNode* newNode = createNode(id, title, priority, deadline, status);
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->count++;
}

/* Delete task by task_id - O(n) */
int deleteTask(LinkedList* list, int id) {
    if (list->head == NULL) return 0;

    TaskNode* current = list->head;
    TaskNode* prev = NULL;

    while (current != NULL && current->task_id != id) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) return 0; // Not found

    if (prev == NULL) {
        list->head = current->next;
        if (list->head == NULL) list->tail = NULL;
    } else {
        prev->next = current->next;
        if (current == list->tail) list->tail = prev;
    }

    free(current);
    list->count--;
    return 1;
}

/* Display all tasks in JSON format for web bridge */
void displayJSON(LinkedList* list, const char* action, const char* message) {
    printf("{\n");
    printf("  \"algorithm\": \"Linked List\",\n");
    printf("  \"action\": \"%s\",\n", action);
    printf("  \"status\": \"success\",\n");
    printf("  \"message\": \"%s\",\n", message);
    printf("  \"node_count\": %d,\n", list->count);
    printf("  \"time_complexity\": {\n");
    printf("    \"insert_head\": \"O(1)\",\n");
    printf("    \"insert_tail\": \"O(1)\",\n");
    printf("    \"delete_by_id\": \"O(n)\",\n");
    printf("    \"traverse\": \"O(n)\"\n");
    printf("  },\n");
    printf("  \"tasks\": [\n");

    TaskNode* curr = list->head;
    int first = 1;
    while (curr != NULL) {
        if (!first) printf(",\n");
        printf("    {\"task_id\": %d, \"title\": \"%s\", \"priority\": \"%s\", \"deadline\": \"%s\", \"status\": \"%s\"}",
               curr->task_id, curr->title, curr->priority, curr->deadline, curr->status);
        first = 0;
        curr = curr->next;
    }
    printf("\n  ]\n");
    printf("}\n");
}

/* Free list memory */
void freeList(LinkedList* list) {
    TaskNode* curr = list->head;
    while (curr != NULL) {
        TaskNode* temp = curr;
        curr = curr->next;
        free(temp);
    }
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

/* Interactive CLI Demonstration Mode */
void runInteractive() {
    LinkedList list;
    initList(&list);
    int choice, id;
    char title[MAX_TITLE], priority[MAX_PRIORITY], deadline[MAX_DATE];

    printf("\n=======================================================\n");
    printf("   ADS PBL: LINKED LIST TASK MANAGER (C IMPLEMENTATION) \n");
    printf("=======================================================\n");

    while (1) {
        printf("\n1. Insert Task\n2. Delete Task by ID\n3. Display Tasks\n4. Exit\nEnter choice: ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1:
                printf("Enter Task ID: ");
                scanf("%d", &id);
                printf("Enter Title: ");
                scanf(" %[^\n]", title);
                printf("Enter Priority (High/Medium/Low): ");
                scanf(" %s", priority);
                printf("Enter Deadline (YYYY-MM-DD): ");
                scanf(" %s", deadline);
                insertTask(&list, id, title, priority, deadline, "Pending");
                printf(">> Task %d successfully inserted into linked list.\n", id);
                break;
            case 2:
                printf("Enter Task ID to delete: ");
                scanf("%d", &id);
                if (deleteTask(&list, id)) {
                    printf(">> Task %d successfully deleted from linked list.\n", id);
                } else {
                    printf(">> Task %d not found in linked list.\n", id);
                }
                break;
            case 3: {
                printf("\n--- CURRENT LINKED LIST NODES (Count: %d) ---\n", list.count);
                TaskNode* temp = list.head;
                int pos = 1;
                while (temp != NULL) {
                    printf("[Node %d] -> [ID: %d | Title: '%s' | Priority: %s | Deadline: %s | Status: %s]\n",
                           pos++, temp->task_id, temp->title, temp->priority, temp->deadline, temp->status);
                    temp = temp->next;
                }
                if (list.count == 0) printf("(List is empty)\n");
                break;
            }
            case 4:
                freeList(&list);
                printf("Exiting Linked List demo.\n");
                return;
            default:
                printf("Invalid choice!\n");
        }
    }
    freeList(&list);
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--interactive") == 0) {
        runInteractive();
        return 0;
    }

    LinkedList list;
    initList(&list);

    /* Command Mode format:
       linked_list insert <id> <title> <priority> <deadline> <status> ...
       or linked_list demo
       or linked_list test
    */
    if (argc >= 2 && strcmp(argv[1], "insert") == 0 && argc >= 6) {
        int id = atoi(argv[2]);
        const char* title = argv[3];
        const char* priority = argv[4];
        const char* deadline = argv[5];
        const char* status = (argc >= 7) ? argv[6] : "Pending";
        insertTask(&list, id, title, priority, deadline, status);
        displayJSON(&list, "insert", "Task node inserted successfully");
    } else if (argc >= 3 && strcmp(argv[1], "batch") == 0) {
        /* Batch format: id|title|priority|deadline|status;... */
        char* token = strtok(argv[2], ";");
        while (token != NULL) {
            int id = 0;
            char title[MAX_TITLE] = {0};
            char priority[MAX_PRIORITY] = {0};
            char deadline[MAX_DATE] = {0};
            char status[MAX_PRIORITY] = {0};
            
            sscanf(token, "%d|%99[^|]|%14[^|]|%24[^|]|%14s", &id, title, priority, deadline, status);
            if (id > 0) {
                insertTask(&list, id, title, priority, deadline, status);
            }
            token = strtok(NULL, ";");
        }

        if (argc >= 5 && strcmp(argv[3], "delete") == 0) {
            int delId = atoi(argv[4]);
            int ok = deleteTask(&list, delId);
            char msg[128];
            snprintf(msg, sizeof(msg), ok ? "Task %d deleted from linked list" : "Task %d not found", delId);
            displayJSON(&list, "delete", msg);
        } else {
            displayJSON(&list, "batch_load", "Linked list built from task records");
        }
    } else {
        /* Default demo */
        insertTask(&list, 101, "Implement Red-Black Tree in C", "High", "2026-11-10", "Pending");
        insertTask(&list, 102, "Merge Sort Benchmark", "Medium", "2026-11-12", "Pending");
        insertTask(&list, 103, "Design Normalized ER Diagram", "High", "2026-11-11", "Pending");
        displayJSON(&list, "demo", "Demo linked list initialized");
    }

    freeList(&list);
    return 0;
}

