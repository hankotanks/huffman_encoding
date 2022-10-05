#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "ll.h"
#include "huffman.h"

int main() {
    TreeNode root = generateFreqTable(); 
    TreeNode temp = root;
    while(temp->parent != NULL) {
            printf("%c : %d\n", temp->symbol, temp->freq);
            temp = temp->parent;
    }
    printf("\n");

    TreeNode master = createHuffmanTree(root);
    displayTree(master);
}

// Checks if a TreeNode has been created for the given symbol
// If it has, increment the TreeNodes `freq` field
int found(TreeNode t, char symbol) {
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

// Helper function for sort that
// Returns true if the linked list is in descending order
int ordered(TreeNode t) {
    TreeNode curr = t;
    while(curr->parent != NULL) {
        if(curr->freq > curr->parent->freq) {
            return 0;
        }

        curr = curr->parent;
    }

    return 1;
}

void sort(TreeNode t) {
    if(t->parent == NULL) { return; }
    while(!ordered(t)) {
        TreeNode prev = t;
        TreeNode curr = t->parent;
        while(curr != NULL) {
            if(curr->freq < prev->freq) {
                int tempFreq = prev->freq;
                int tempChar = prev->symbol;

                prev->freq = prev->parent->freq;
                prev->symbol = prev->parent->symbol;

                prev->parent->freq = tempFreq;
                prev->parent->symbol = tempChar;
            }

            prev = curr;
            curr = prev->parent;
        }
    }
}

// Does not return a proper tree
// Instead, yields a chain of TreeNodes with character occurrences
// This string of nodes is not bidirectional
// Only the `parent` field is set
TreeNode generateFreqTable() {
    TreeNode parent = newTreeNode(0);

    char temp;
    while(scanf("%c", &temp) != EOF) {
        if(!found(parent, temp) && temp != 10) {
            TreeNode curr = newTreeNode(temp);
            curr->parent = parent;
            parent = curr;
        }
    }    

    sort(parent);

    return parent;
}

TreeNode createHuffmanTree(TreeNode t) {
    if(t->parent == NULL) { return t; }

    TreeNode curr = t;
    while(curr->parent->parent != NULL) {
       TreeNode sum = newTreeNode(0);
       sum->freq = curr->freq + curr->parent->freq; 
       sum->parent = curr->parent->parent;
       sum->left = curr;
       sum->right = curr->parent;
       curr->parent->parent = sum;
       curr->parent = sum;
       curr = sum;
    }

    return curr;
}
