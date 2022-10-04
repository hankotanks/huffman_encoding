#include<stdlib.h>
#include "ll.h"

typedef struct node {
    int occurences;
    char symbol;
    TreeNode* left;
    TreeNode* right;
    TreeNode* parent;
} TreeNode;


TreeNode* createTree() {
    return NULL;
}

int isEmptyTree(TreeNode* t) {
    return (t == NULL);
}
void displayTreeHelper(TreeNode* t, int indent) {
    if(t != NULL) {
        int i;
        for(i = 0; i < indent; i++) { printf(" "); }
        printf("%c: %d", p->symbol, p->freq);

        displayTree(t->left, indent + 4);
        displayTree(t->right, indent + 4);
    }
}

void displayTree(TreeNode* t) {
    displayTreeHelper(t, 0);
}
void addTree(TreeNode** t, int val);
void freeTree(TreeNode* t);
