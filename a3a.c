#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

struct ListNode {
    int val;
    struct ListNode* next;
};

struct LinkedListArray {
    struct ListNode** lists;
    int size;
    int capacity;
};

struct ListNode* createNode(int val) {
    struct ListNode* node = malloc(sizeof(struct ListNode));
    if (!node) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
    node->val = val;
    node->next = NULL;
    return node;
}

struct ListNode* arrayToLinkedList(int* arr, int size) {
    if (size == 0) return NULL;
    struct ListNode* head = createNode(arr[0]);
    struct ListNode* curr = head;
    for (int i = 1; i < size; i++) {
        curr->next = createNode(arr[i]);
        curr = curr->next;
    }
    return head;
}

struct ListNode* mergeKLists(struct ListNode** lists, int k) {
    if (k == 0) return NULL;
    int validCount = 0;
    for (int i = 0; i < k; i++)
        if (lists[i] != NULL) validCount++;
    if (validCount == 0) return NULL;

    struct ListNode dummy = {0, NULL};
    struct ListNode* curr = &dummy;

    while (validCount > 0) {
        int minVal = INT_MAX;
        int minIndex = -1;
        for (int i = 0; i < k; i++) {
            if (lists[i] && lists[i]->val < minVal) {
                minVal = lists[i]->val;
                minIndex = i;
            }
        }
        if (minIndex == -1) break;
        curr->next = createNode(lists[minIndex]->val);
        curr = curr->next;

        struct ListNode* temp = lists[minIndex];
        lists[minIndex] = lists[minIndex]->next;
        free(temp);

        if (lists[minIndex] == NULL)
            validCount--;
    }

    return dummy.next;
}

void printLinkedList(struct ListNode* head) {
    printf("[");
    int first = 1;
    for (struct ListNode* curr = head; curr; curr = curr->next) {
        if (!first) printf(", ");
        printf("%d", curr->val);
        first = 0;
    }
    printf("]\n");
}

void freeLinkedList(struct ListNode* head) {
    while (head) {
        struct ListNode* tmp = head;
        head = head->next;
        free(tmp);
    }
}

struct LinkedListArray* initLinkedListArray() {
    struct LinkedListArray* lla = malloc(sizeof(struct LinkedListArray));
    if (!lla) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
    lla->capacity = 10;
    lla->size = 0;
    lla->lists = malloc(lla->capacity * sizeof(struct ListNode*));
    if (!lla->lists) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
    return lla;
}

void addLinkedList(struct LinkedListArray* lla, struct ListNode* list) {
    if (lla->size >= lla->capacity) {
        lla->capacity *= 2;
        lla->lists = realloc(lla->lists, lla->capacity * sizeof(struct ListNode*));
        if (!lla->lists) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
    }
    lla->lists[lla->size++] = list;
}

void freeLinkedListArray(struct LinkedListArray* lla) {
    if (!lla) return;
    for (int i = 0; i < lla->size; i++) {
        freeLinkedList(lla->lists[i]);
    }
    free(lla->lists);
    free(lla);
}

int parseLine(char* line, int** arr) {
    int capacity = 10;
    *arr = malloc(capacity * sizeof(int));
    if (!*arr) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }

    int count = 0;
    char* ptr = line;

    // Skip until '['
    while (*ptr && *ptr != '[') ptr++;
    if (*ptr != '[') {
        free(*arr);
        *arr = NULL;
        return 0;
    }
    ptr++; // skip '['

    while (*ptr && *ptr != ']') {
        while (*ptr && isspace(*ptr)) ptr++;
        if (*ptr == ']') break;
        if (*ptr == ',') { ptr++; continue; }

        if (isdigit(*ptr) || (*ptr == '-' && isdigit(*(ptr + 1)))) {
            if (count >= capacity) {
                capacity *= 2;
                *arr = realloc(*arr, capacity * sizeof(int));
                if (!*arr) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
            }
            char* endptr;
            int val = (int)strtol(ptr, &endptr, 10);
            if (endptr == ptr) break;
            (*arr)[count++] = val;
            ptr = endptr;
        } else {
            ptr++;
        }
    }

    return count;
}

int main() {
    struct LinkedListArray* lla = initLinkedListArray();
    char line[2048];

    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;

        char* ptr = line;
        while (*ptr && isspace(*ptr)) ptr++;
        if (*ptr != '[') continue;

        int* arr = NULL;
        int size = parseLine(line, &arr);

        struct ListNode* list = arrayToLinkedList(arr, size);
        addLinkedList(lla, list);

        free(arr);
    }

    if (lla->size == 0) {
        printf("[]\n");
        freeLinkedListArray(lla);
        return 0;
    }

    struct ListNode* merged = mergeKLists(lla->lists, lla->size);

    printLinkedList(merged);

    freeLinkedList(merged);
    freeLinkedListArray(lla);

    return 0;
}
