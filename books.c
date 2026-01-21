#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Book {
    int id;
    char title[64];
    char category[32];
    int totalCopies;
    int available;
    struct Book *left, *right;
} Book;

Book* insertBook(Book* root, int id, const char* title, const char* category, int copies);
Book* searchBook(Book* root, int id);
Book* deleteBook(Book* root, int id);
void recommend_by_category_reset(Book* root, const char* category, int limit);
void displayBooks(Book* root);

Book* createNode(int id, const char* title, const char* category, int copies) {
    Book* n = malloc(sizeof(Book));
    n->id = id;
    strncpy(n->title, title, 63); n->title[63]=0;
    strncpy(n->category, category, 31); n->category[31]=0;
    n->totalCopies = copies;
    n->available = copies;
    n->left = n->right = NULL;
    return n;
}

Book* insertBook(Book* root, int id, const char* title, const char* category, int copies) {
    if (!root) return createNode(id, title, category, copies);
    if (id < root->id) root->left = insertBook(root->left, id, title, category, copies);
    else if (id > root->id) root->right = insertBook(root->right, id, title, category, copies);
    else { root->totalCopies += copies; root->available += copies; }
    return root;
}

Book* searchBook(Book* root, int id) {
    if (!root) return NULL;
    if (root->id == id) return root;
    if (id < root->id) return searchBook(root->left, id);
    return searchBook(root->right, id);
}

Book* minNode(Book* r){ while(r->left) r=r->left; return r; }

Book* deleteBook(Book* root, int id){
    if(!root) return NULL;
    if(id<root->id) root->left=deleteBook(root->left,id);
    else if(id>root->id) root->right=deleteBook(root->right,id);
    else{
        if(!root->left){ Book* t=root->right; free(root); return t; }
        if(!root->right){ Book* t=root->left; free(root); return t; }
        Book* m=minNode(root->right);
        root->id=m->id;
        strcpy(root->title,m->title);
        strcpy(root->category,m->category);
        root->totalCopies=m->totalCopies;
        root->available=m->available;
        root->right=deleteBook(root->right,m->id);
    }
    return root;
}

static int rec_count;

void rec_cat(Book* r,const char* c,int L){
    if(!r||rec_count>=L) return;
    rec_cat(r->left,c,L);
    if(rec_count<L && strcmp(r->category,c)==0){
        printf("Recommend %d %s %s\n",r->id,r->title,r->category);
        rec_count++;
    }
    rec_cat(r->right,c,L);
}

void recommend_by_category_reset(Book* r,const char* c,int L){
    rec_count=0;
    rec_cat(r,c,L);
}

void displayBooks(Book* root){
    if(!root) return;
    displayBooks(root->left);
    printf("%d %s %s %d/%d\n",root->id,root->title,root->category,root->available,root->totalCopies);
    displayBooks(root->right);
}
