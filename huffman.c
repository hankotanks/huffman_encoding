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

    
    if(mode) {
        // This is a one-way linked list of character frequencies
        TreeNode root = createFreqTable(argv[2]); 

        // Build an array of leaf nodes
        int leafCount = leavesCount(root);
        
        // Construct the huffman tree
        TreeNode master = createHuffmanTree(root);
        TreeNode* leafNodes = leaves(master, leafCount);

        encode(argv[2], &leafNodes, leafCount);
    } else {
        int len = strlen(argv[2]);
        if(len <= 4) {
            printf("Error: File must have both name and extension\n");
            exit(1);
        }

        const char* ext = &(argv[2])[len - 4];
        const char* huf = ".huf";
        if(strcmp(ext, huf) != 0) {
            printf("Error: Invalid file extension (should be *.huf)");
            exit(1);
        }

        char temp[len - 4];
        int i;
        for(i = 0; i < len - 4; i++) { temp[i] = argv[2][i]; }
        temp[len - 4] = '\0';

        TreeNode root = createFreqTable(temp);
        TreeNode master = createHuffmanTree(root);

        
        decode(argv[2], master);
        // Free all TreeNodes when encoding is complete
        freeTree(master);
   }

}

// Checks if a TreeNode has been created for the given symbol
// If it has, increment the TreeNodes `freq` field
int found(TreeNode t, char symbol) {
    TreeNode curr = t;
    while(curr != NULL) {
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
                prev->freq = curr->freq;
                prev->symbol = curr->symbol;
                prev->left = curr->left;
                prev->right = curr->right;

                // Move values from current to preceding
                curr->freq = tempFreq;
                curr->symbol = tempChar;
                curr->left = tempLeft;
                curr->right = tempRight;
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
    TreeNode parent = NULL;

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

void leavesTraversal(TreeNode t, TreeNode** leaves, int* index) {
    if(t == NULL) { return; }
    
    if(t->left == NULL && t->right == NULL) {
        (*leaves)[*index] = t;
        (*index)++;
    } else {
        leavesTraversal(t->left, leaves, index);
        leavesTraversal(t->right, leaves, index);
    }
}



TreeNode* leaves(TreeNode t, int leavesCount) {
    TreeNode* leaves = (TreeNode*) malloc (sizeof(TreeNode) * leavesCount);

    int index = 0;
    leavesTraversal(t, &leaves, &index);

    return leaves;
}

void links(TreeNode t) {
    if(t->left != NULL) {
        t->left->parent = t;
        links(t->left);
    }

    if(t->right != NULL) {
        t->right->parent = t;
        links(t->right);
    }
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

        // The internal node is now the bottom-most node
        curr = join;

        // Sort the list before the next iteration
        sort(curr);
        links(curr);
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
    fo = fopen(output, "w");

    char buffer = 0;
    int bufferLen = 8;
    char temp;
    while(fscanf(fi, "%c", &temp) != EOF) {
        TreeNode curr;
        int i;
        for(i = 0; i < leavesCount; i++) {
            curr = (*leaves)[i];
            if(curr->symbol == temp) {
                unsigned long charBuffer = 0;
                int charBufferLen = 0;

                while(curr->parent != NULL) {
                    if(curr->parent->right->symbol == curr->symbol && curr->parent->right->freq == curr->freq) { 
                        charBuffer++;
                    }

                    charBufferLen++;
                    curr = curr->parent;
                    if(curr->parent != NULL) {
                        charBuffer <<= 1;
                    }
                }

                printf("%d: ", charBuffer);
                printf("%d, ", charBufferLen);

                while(charBufferLen > 0) {
                    int val = (charBuffer & 1);
                    charBuffer >>= 1;
                    charBufferLen--;

                    buffer |= (val << bufferLen - 1);
                    bufferLen--;

                    if(bufferLen == 0) {
                        fputc(buffer, fo);
                        buffer = 0;
                        bufferLen = 8;
                    }
                }

                charBuffer = 0;
            }
        }
    } 
    

    if(bufferLen != 8) {
        fputc(buffer, fo);
    }

    fclose(fi);
    fclose(fo);
}

void decode(char* file, TreeNode root) {
    // TODO
    FILE* fi;
    fi = fopen(file, "r");

    int len = strlen(file);
    char temp[len];
    strcpy(temp, file);
    temp[len - 3] = 'd';
    temp[len - 2] = 'e';
    temp[len - 1] = 'c';

    FILE* fo;
    fo = fopen(temp, "w");
    
    TreeNode curr = root;
    char buffer;
    int bufferLen;
    while(fscanf(fi, "%c", &buffer) != EOF) {

        for (int i = 7; i >= 0; --i) { putchar( (buffer & (1 << i)) ? '1' : '0' ); } putchar('\n');

        bufferLen = 7;
        while(bufferLen >= 0) {
            if(curr->left == NULL && curr->right == NULL) {
                fputc(curr->symbol, fo);
                curr = root;
                continue;
            } 

            if((buffer >> bufferLen) & 1) {
                curr = curr->right;
            } else {
                curr = curr->left;
            }

            bufferLen -= 1;
        }
    }

    fclose(fi);
    fclose(fo);
}
