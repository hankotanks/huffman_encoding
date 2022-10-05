#include <stdlib.h>
#include <stdio.h>
#include "ll.h"

TreeNode newTreeNode(char symbol) {
    TreeNode temp = (TreeNode) calloc (1, sizeof(struct node));
    temp->symbol = symbol;
    temp->freq = 1;
    return temp; 
}

int isEmptyTree(TreeNode t) {
    return (t == NULL);
}

void displayTreeHelper(TreeNode t, int indent) {
    if(t == NULL) { return; }

    int i;
    for(i = 0; i < indent; i++) { printf(" "); }

    char temp;
    if(t->symbol != 0) { temp = t->symbol; } else { temp = '*'; }
    printf("[%c] : %d\n", temp, t->freq);

    displayTreeHelper(t->left, indent + 3);
    displayTreeHelper(t->right, indent + 3);
}

void displayTree(TreeNode t) {
    displayTreeHelper(t, 0);
}

void freeTree(TreeNode t) {
    if(t == NULL) { return; }

    freeTree(t->left);
    freeTree(t->right);
    free(t);
}
