#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "ll.h"
#include "huffman.h"

int main() {
    TreeNode root = generateFreqTable(); 
    while(root->parent != NULL) {
        displayTree(root);
        root = root->parent;
    }
}

int treeNodeFound(TreeNode t, char symbol) {
    TreeNode curr = t;
    while(curr->symbol != 0) {
        if(curr->symbol == symbol) {
            curr->freq += 1;
            return 1;
        }
        curr = curr->parent;
    }

    return 0;
}

// Does not return a proper tree
// Instead, yields a chain of TreeNodes with character occurrences
// This string of nodes is not bidirectional
// Only the `parent` field is set
TreeNode generateFreqTable() {
    TreeNode parent = newTreeNode(0);

    char temp;
    while(scanf("%c", &temp) != EOF) {
        if(!treeNodeFound(parent, temp) && temp != 10) {
            TreeNode curr = newTreeNode(temp);
            curr->parent = parent;
            parent = curr;
        }
    }    

    return parent;
}

