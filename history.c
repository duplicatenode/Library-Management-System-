#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NAME_LEN 50
#define REC_LEN 160
#define MAX_HISTORY 500
#define MAX_BORROW 1000
#define LOAN_PERIOD 14
#define MAX_RENEWALS 2

typedef struct Book{
    int id;
    char title[64];
    char category[32];
    int totalCopies;
    int available;
    struct Book *left, *right;
} Book;

typedef struct Request {
    char name[NAME_LEN];
    int bookId;
} Request;

typedef struct Queue {
    Request arr[120];
    int front, rear;
} Queue;

typedef struct {
    char recs[MAX_HISTORY][REC_LEN];
    int top;
} Stack;

typedef struct {
    int bookId;
    char username[NAME_LEN];
    int borrowedDay;
    int dueDay;
    int returnedDay;
    int renewals;
    int active;
} BorrowRecord;

Book* root = NULL;
Queue waitQueue;
Stack historyStack;
int today = 0;

BorrowRecord borrowDB[MAX_BORROW];
int borrowCount = 0;

extern int dequeue_first_for_book(Queue*, char*, int);
extern Book* searchBook(Book*, int);
extern void recommend_by_category_reset(Book*, const char*, int);

void initStack(Stack *s) {
    s->top = -1;
}

void pushStack(Stack* s, const char* txt) {
    if (s->top >= MAX_HISTORY - 1) return;
    s->top++;
    strncpy(s->recs[s->top], txt, REC_LEN - 1);
    s->recs[s->top][REC_LEN - 1] = 0;
}

void displayStack(Stack* s) {
    if (s->top < 0) { printf("No transactions\n"); return; }
    for (int i = s->top; i >= 0; i--) printf("%s\n", s->recs[i]);
}

void add_borrow_record(int id, const char* user) {
    BorrowRecord* r = &borrowDB[borrowCount++];
    r->bookId = id;
    strncpy(r->username, user, NAME_LEN-1);
    r->username[NAME_LEN-1] = 0;
    r->borrowedDay = today;
    r->dueDay = today + LOAN_PERIOD;
    r->returnedDay = -1;
    r->renewals = 0;
    r->active = 1;
}

int borrow_book(int id, const char* user) {
    Book* b = searchBook(root, id);
    if (!b) { printf("Not found\n"); return 0; }
    if (b->available <= 0) {
        printf("None available, added to waiting\n");
        extern void enqueue(Queue*, const char*, int);
        enqueue(&waitQueue, user, id);
        return 0;
    }
    b->available--;
    add_borrow_record(id, user);
    char rec[200];
    sprintf(rec, "ISSUE %s %d day %d due %d", user, id, today, today + LOAN_PERIOD);
    pushStack(&historyStack, rec);
    printf("Issued %d to %s\n", id, user);
    return 1;
}

int return_book(const char* user, int id) {
    for (int i = 0; i < borrowCount; i++) {
        if (borrowDB[i].active &&
            borrowDB[i].bookId == id &&
            strcmp(borrowDB[i].username, user) == 0) {
            borrowDB[i].active = 0;
            borrowDB[i].returnedDay = today;
            Book* b = searchBook(root, id);
            if (b) b->available++;
            char rec[200];
            sprintf(rec, "RETURN %s %d day %d", user, id, today);
            pushStack(&historyStack, rec);
            printf("Returned %d by %s\n", id, user);
            char nxt[50];
            if (dequeue_first_for_book(&waitQueue, nxt, id)) {
                printf("Auto-issue to %s\n", nxt);
                borrow_book(id, nxt);
            }
            return 1;
        }
    }
    printf("No active borrow\n");
    return 0;
}

int renew_book(const char* user, int id) {
    for (int i = 0; i < borrowCount; i++) {
        if (borrowDB[i].active &&
            borrowDB[i].bookId == id &&
            strcmp(borrowDB[i].username, user) == 0) {
            if (today > borrowDB[i].dueDay) {
                printf("Overdue\n");
                return 0;
            }
            if (borrowDB[i].renewals >= MAX_RENEWALS) {
                printf("Max renewals\n");
                return 0;
            }
            borrowDB[i].dueDay += LOAN_PERIOD;
            borrowDB[i].renewals++;
            char rec[200];
            sprintf(rec, "RENEW %s %d day %d newDue %d", user, id, today, borrowDB[i].dueDay);
            pushStack(&historyStack, rec);
            printf("Renewed\n");
            return 1;
        }
    }
    printf("Not found\n");
    return 0;
}

void show_overdues() {
    int f = 0;
    printf("Overdues:\n");
    for (int i = 0; i < borrowCount; i++) {
        if (borrowDB[i].active && borrowDB[i].dueDay < today) {
            printf("%s -> %d overdue %d days\n",
                   borrowDB[i].username,
                   borrowDB[i].bookId,
                   today - borrowDB[i].dueDay);
            f = 1;
        }
    }
    if (!f) printf("None\n");
}

void show_user_history(const char* user) {
    int f = 0;
    for (int i = 0; i < borrowCount; i++) {
        if (strcmp(borrowDB[i].username, user) == 0) {
            f = 1;
            printf("Book %d borrowed %d due %d ",
                   borrowDB[i].bookId,
                   borrowDB[i].borrowedDay,
                   borrowDB[i].dueDay);
            if (borrowDB[i].active) printf("ACTIVE\n");
            else printf("Returned %d\n", borrowDB[i].returnedDay);
        }
    }
    if (!f) printf("No history\n");
}
