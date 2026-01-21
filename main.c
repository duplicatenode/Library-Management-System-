#include <stdio.h>
#include <string.h>

#define NAME_LEN 50
#define REC_LEN 160

typedef struct Book {
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
    char recs[500][REC_LEN];
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

/* functions/vars implemented in other .c files */
extern Book* root;
extern Queue waitQueue;
extern Stack historyStack;
extern int today;

extern BorrowRecord borrowDB[];
extern int borrowCount;

extern Book* insertBook(Book* root, int id, const char* title, const char* category, int copies);
extern Book* deleteBook(Book* root, int id);
extern void displayBooks(Book* root);
extern Book* searchBook(Book* root, int id);
extern void recommend_by_category_reset(Book* root, const char* category, int limit);

extern void initQueue(Queue *q);
extern void enqueue(Queue *q, const char* name, int bookId);
extern void displayQueue(Queue *q);

extern void initStack(Stack *s);
extern void displayStack(Stack *s);

extern int borrow_book(int bookId, const char* username);
extern int return_book(const char* username, int bookId);
extern int renew_book(const char* username, int bookId);
extern void show_overdues();
extern void show_user_history(const char* username);

void seed_data() {
    root = insertBook(root, 101, "C_Programming", "CS", 3);
    root = insertBook(root, 102, "DSA", "CS", 2);
    root = insertBook(root, 201, "Motivation", "SelfHelp", 4);
    root = insertBook(root, 301, "WorldHistory", "History", 1);
}

int main() {
    initQueue(&waitQueue);
    initStack(&historyStack);
    seed_data();

    int choice, bid;
    char uname[NAME_LEN], title[64], cat[32];

    while (1) {
        printf("\nDay=%d\n", today);
        printf("1 Add Book\n2 Delete Book\n3 Show Books\n");
        printf("4 Borrow\n5 Return\n6 Renew\n");
        printf("7 Add Waiting\n8 Show Waiting\n9 Show Transactions\n");
        printf("10 Recommend\n11 Overdues\n12 User History\n13 Next Day\n14 Exit\n");
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) { while(getchar()!='\n'); continue; }

        switch (choice) {
            case 1: {
                int copies;
                printf("Enter id title category copies: ");
                if (scanf("%d %63s %31s %d", &bid, title, cat, &copies) != 4) { while(getchar()!='\n'); break; }
                root = insertBook(root, bid, title, cat, copies);
            } break;

            case 2:
                printf("Enter id: ");
                if (scanf("%d", &bid) != 1) { while(getchar()!='\n'); break; }
                root = deleteBook(root, bid);
                break;

            case 3:
                displayBooks(root);
                break;

            case 4:
                printf("username id: ");
                if (scanf("%49s %d", uname, &bid) != 2) { while(getchar()!='\n'); break; }
                borrow_book(bid, uname);
                break;

            case 5:
                printf("username id: ");
                if (scanf("%49s %d", uname, &bid) != 2) { while(getchar()!='\n'); break; }
                return_book(uname, bid);
                break;

            case 6:
                printf("username id: ");
                if (scanf("%49s %d", uname, &bid) != 2) { while(getchar()!='\n'); break; }
                renew_book(uname, bid);
                break;

            case 7:
                printf("username id: ");
                if (scanf("%49s %d", uname, &bid) != 2) { while(getchar()!='\n'); break; }
                enqueue(&waitQueue, uname, bid);
                break;

            case 8:
                displayQueue(&waitQueue);
                break;

            case 9:
                displayStack(&historyStack);
                break;

            case 10:
                printf("Enter username: ");
                if (scanf("%49s", uname) != 1) { while(getchar()!='\n'); break; }
                printf("Mode 1(category) 2(history): ");
                {
                    int m;
                    if (scanf("%d", &m) != 1) { while(getchar()!='\n'); break; }
                    if (m == 1) {
                        printf("Category: ");
                        if (scanf("%31s", cat) != 1) { while(getchar()!='\n'); break; }
                        recommend_by_category_reset(root, cat, 3);
                    } else {
                        int f = 0;
                        for (int i = borrowCount - 1; i >= 0; i--) {
                            if (strcmp(borrowDB[i].username, uname) == 0) {
                                Book* b = searchBook(root, borrowDB[i].bookId);
                                if (b) {
                                    recommend_by_category_reset(root, b->category, 3);
                                    f = 1;
                                    break;
                                }
                            }
                        }
                        if (!f) printf("No history.\n");
                    }
                }
                break;

            case 11:
                show_overdues();
                break;

            case 12:
                printf("username: ");
                if (scanf("%49s", uname) != 1) { while(getchar()!='\n'); break; }
                show_user_history(uname);
                break;

            case 13:
                today++;
                printf("Day -> %d\n", today);
                break;

            case 14:
                return 0;

            default:
                printf("Invalid\n");
        }
    }
    return 0;
}
