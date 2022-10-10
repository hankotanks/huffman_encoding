#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "ll.h"
#include "huffman.h"

int main(int argc, char* argv[]) {
    if(argc != 3) { 
        printf("Error: Incorrect argument format\n"); 
        exit(1);
    }
    
    int mode;
    if(strcmp(argv[1], "-e") == 0) {
        mode = 1;
    } else if(strcmp(argv[1], "-d") == 0) { 
        mode = 0;
    } else {
        printf("Error: Invalid command flag\n");
        exit(1);
    }     

    // This is a one-way linked list of character frequencies
    TreeNode root = createFreqTable(argv[2]); 

    // Build an array of leaf nodes
    int leafCount = leavesCount(root);
    TreeNode* leafNodes = leaves(root, leafCount);
    
    // Construct the huffman tree
    TreeNode master = createHuffmanTree(root);

    for(int i = 0; i < leafCount; i++) {
        printf("%c", leafNodes[i]->symbol);
    }
    printf("\n");

    if(mode) {
        encode(argv[2], &leafNodes, leafCount);
    } else {
        decode(argv[2], master);
    }

    // Free all TreeNodes when encoding is complete
    freeTree(master);
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
                // Copy TreeNode content
                int tempFreq = prev->freq;
                int tempChar = prev->symbol;
                TreeNode tempLeft = prev->left;
                TreeNode tempRight = prev->right;

                // Assign values from preceding TreeNode
                prev->freq = prev->parent->freq;
                prev->symbol = prev->parent->symbol;
                prev->left = prev->parent->left;
                prev->right = prev->parent->right;

                // Move values from current to preceding
                prev->parent->freq = tempFreq;
                prev->parent->symbol = tempChar;
                prev->parent->left = tempLeft;
                prev->parent->right = tempRight;
            }

            // Advance to next pair of TreeNodes
            prev = curr;
            curr = prev->parent;
        }
    }
}

// Does not return a proper tree
// Instead, yields a chain of TreeNodes with character occurrences
// This string of nodes is not bidirectional
// Only the `parent` field is set
TreeNode createFreqTable(char* file) {
    FILE* fi;
    fi = fopen(file, "r");
    TreeNode parent = newTreeNode(0);

    char tempChar;
    while(fscanf(fi, "%c", &tempChar) != EOF) {
        if(!found(parent, tempChar)) {
            TreeNode curr = newTreeNode(tempChar);
            curr->parent = parent;
            parent = curr;
        }
    }    

    // Ensure that list is in descending frequency
    sort(parent);

    // Remove the sentinel node from the freqTable
    TreeNode temp = parent;
    while(temp->parent != NULL) {
        if(temp->parent->symbol == 0) {
            TreeNode removed = temp->parent;
            temp->parent = removed->parent;
            free(removed);
        }

        temp = temp->parent;
    }

    // Free the frequency table's sentinel
    return parent;
}

int leavesCount(TreeNode t) {
    TreeNode curr = t;

    int count = 0;
    while(curr != NULL) { 
        count++;
        curr = curr->parent;
    }

    return count;
}

TreeNode* leaves(TreeNode t, int leavesCount) {
    TreeNode* leaves = (TreeNode*) malloc (sizeof(TreeNode) * leavesCount);
    TreeNode temp = t;
    
    int i;
    for(i = 0; i < leavesCount; i++) {
        leaves[i] = temp;
        temp = temp->parent;
    }

    return leaves;
}

TreeNode createHuffmanTree(TreeNode t) {
    TreeNode curr = t;
    while(curr->parent != NULL) {
        TreeNode first = curr;
        TreeNode second = curr->parent;
        
        // The new internal node
        TreeNode join = newTreeNode(0);
        join->freq = first->freq + second->freq;
        join->parent = second->parent;
        join->left = first;
        join->right = second;

        // Set the parents of the two child nodes
        first->parent = join;
        second->parent = join;   

        // The internal node is now the bottom-most node
        curr = join;

        // Sort the list before the next iteration
        sort(curr);
    }

    return curr;
}

void encode(char* file, TreeNode** leaves, int leavesCount) {
    FILE* fi;
    fi = fopen(file, "r");
    if(fi == NULL) {
        printf("Error: Couldn't open file\n");
        exit(1);
    }

    // Construct the output file's name
    int len = strlen(file);
    char output[len + 4];
    strcpy(output, file);
    output[len + 0] = '.';
    output[len + 1] = 'h';
    output[len + 2] = 'u';
    output[len + 3] = 'f';

    FILE* fo;
    fo = fopen(output, "a");

    // TODO
    unsigned short int buffer = 0;
    char temp;
    while(fscanf(fi, "%c", &temp) != EOF) {
        TreeNode curr;
        int i;
        for(i = 0; i < leavesCount; i++) {
            curr = (*leaves)[i];
            if(curr->symbol == temp) {
            }
        }
    } 
    
    fclose(fi);
    fclose(fo);
}

void decode(char* file, TreeNode root) {
    // TODO
}
