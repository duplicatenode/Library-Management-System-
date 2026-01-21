#include <stdio.h>
#include <string.h>

#define QUEUE_SIZE 120
#define NAME_LEN 50

typedef struct {
    char name[NAME_LEN];
    int bookId;
} Request;

typedef struct {
    Request arr[QUEUE_SIZE];
    int front, rear;
} Queue;

void initQueue(Queue *q){ q->front=q->rear=-1; }
int isQueueEmpty(Queue*q){ return q->front==-1; }
int isQueueFull(Queue*q){ return q->rear==QUEUE_SIZE-1; }

void enqueue(Queue*q,const char*name,int id){
    if(isQueueFull(q)){ printf("Waiting full\n"); return; }
    if(q->front==-1) q->front=0;
    q->rear++;
    strncpy(q->arr[q->rear].name,name,49);
    q->arr[q->rear].name[49]=0;
    q->arr[q->rear].bookId=id;
    printf("%s added for %d\n",name,id);
}

int dequeue(Queue*q,char*out,int*bid){
    if(isQueueEmpty(q)) return 0;
    if(out) strcpy(out,q->arr[q->front].name);
    if(bid) *bid=q->arr[q->front].bookId;
    if(q->front==q->rear) q->front=q->rear=-1;
    else q->front++;
    return 1;
}

int dequeue_first_for_book(Queue*q,char*out,int id){
    if(isQueueEmpty(q)) return 0;
    int found=0,ti=0;
    Request temp[QUEUE_SIZE];
    for(int i=q->front;i<=q->rear;i++){
        if(!found && q->arr[i].bookId==id){
            if(out) strcpy(out,q->arr[i].name);
            found=1;
            continue;
        }
        temp[ti++]=q->arr[i];
    }
    if(!found) return 0;
    if(ti==0){ q->front=q->rear=-1; return 1; }
    for(int i=0;i<ti;i++) q->arr[i]=temp[i];
    q->front=0; q->rear=ti-1;
    return 1;
}

void displayQueue(Queue*q){
    if(isQueueEmpty(q)){ printf("Empty\n"); return; }
    for(int i=q->front;i<=q->rear;i++)
        printf("%s -> %d\n",q->arr[i].name,q->arr[i].bookId);
}
