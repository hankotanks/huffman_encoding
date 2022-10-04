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
    if(t != NULL) {
        int i;
        for(i = 0; i < indent; i++) { printf(" "); }
        printf("%c: %d\n", t->symbol, t->freq);

        displayTreeHelper(t->left, indent + 4);
        displayTreeHelper(t->right, indent + 4);
    }
}

void displayTree(TreeNode t) {
    displayTreeHelper(t, 0);
}

void freeTree(TreeNode t) {
    if(t != NULL) {
        freeTree(t->left);
        freeTree(t->right);
        free(t);
    }
}
