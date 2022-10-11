#include <stdlib.h>
#include <stdio.h>
#include "ll.h"

// Helper method to initialize a newTreeNode
TreeNode newTreeNode(char symbol) {
    TreeNode temp = (TreeNode) calloc (1, sizeof(struct node));
    temp->symbol = symbol;
    temp->freq = 1;
    temp->left = NULL;
    temp->right = NULL;
    temp->parent = NULL;
    return temp; 
}

// Recurses through the tree, displaying children through indentation
void displayTreeHelper(TreeNode root, int indent) {
    if(root == NULL) { return; }

    int i;
    for(i = 0; i < indent; i++) { printf(" "); }

    // Print the node's symbol,
    // unless it's a parent node, then print *
    char temp;
    if(root->symbol != 0) { temp = root->symbol; } else { temp = '*'; }
    printf("[%c] : %d\n", temp, root->freq);

    // Recurse on node's children
    displayTreeHelper(root->left, indent + 3);
    displayTreeHelper(root->right, indent + 3);
}

// Calls the recursive display function above
void displayTree(TreeNode root) {
    displayTreeHelper(root, 0);
}

// Recursively frees a tree, takes its root as an argument
void freeTree(TreeNode root) {
    if(root == NULL) { return; }

    freeTree(root->left);
    freeTree(root->right);
    free(root);
}
