#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "tree.h"
#include "huffman.h"

#define UL_SIZE sizeof(unsigned long) * 8

int main(int argc, char* argv[]) {
    // Make sure the right number of arguments are present
    if(argc < 2) { 
        printf("Error: Invalid argument format\n"); 
        exit(1);
    }

    FILE* temp;
    
    // Check if the user wants to encode or decode
    int mode;
    if(strcmp(argv[1], "-e") == 0) {
        if(argc != 3) {
            printf("Error: Encoder expects 1 argument\n");
            exit(1);
        }

        // Make sure the target file exists
        temp = fopen(argv[2], "rb");
        if(temp == NULL) { // If file doesn't exist
            printf("Error: Couldn't open file\n");
            exit(1);
        } else {
            fclose(temp);
        }

        mode = 1;
    } else if(strcmp(argv[1], "-d") == 0) { 
        mode = 0;
        if(argc != 4) {
            printf("Error: Decoder expects 2 arguments\n");
            exit(1);
        }

        // Make sure the target file exists
        temp = fopen(argv[2], "rb");
        if(temp == NULL) { // If file doesn't exist
            printf("Error: Couldn't open file\n");
            exit(1);
        } else {
            fclose(temp);
        }

        temp = fopen(argv[3], "rb");
        if(temp == NULL) {
            printf("Error: Couldn't open file\n");
            exit(1);
        } else {
            fclose(temp);
        }

        int failed = 0;

        // Confirm that the file has the proper extension
        int len = strlen(argv[2]);
        const char* target_ext = &(argv[2])[len - 4];
        const char* huf = ".huf";
        if(strcmp(target_ext, huf) != 0) {
            printf("Error: Invalid file extension (should be .huf)\n");
            failed = 1;
        }

        len = strlen(argv[3]);
        const char* freq_ext = &(argv[3])[len - 5];
        const char* freq = ".freq";
        if(strcmp(freq_ext, freq) != 0) {
            printf("Error: Invalid file extension (should be .freq)\n");
            failed = 1;
        }

        if(failed) { exit(1); }
    } else { // Exit if the command flag isn't `-e` or `-d`
        printf("Error: Invalid command flag\n");
        exit(1);
    }     

    // `head` points to the 1st node of the freq linked list
    // `root` points to the root of the huffman tree
    TreeNode head;
    TreeNode root;
    
    if(mode) {
        // This is a one-way linked list of character frequencies
        head = createFreqTable(argv[2]); 

        // Save the freqTable to another file
        writeFreqTable(argv[2], head);

        // Build an array of leaf nodes
        int leafCount = leavesCount(head);
        
        // Construct the huffman tree
        root = createHuffmanTree(head);
        TreeNode* leafNodes = leaves(root, leafCount);
        
        // Encode using the tree's leaves
        encode(argv[2], &leafNodes, leafCount);
    } else {
        // Build the tree from the freq table file
        TreeNode head = readFreqTable(argv[3]);
        TreeNode root = createHuffmanTree(head);

        // Decode the target file
        decode(argv[2], root);
   }
   
   // Free all TreeNodes when encoding is complete
   freeTree(root);
}

// Checks if a TreeNode has been created for the given symbol
// If it has, increment the TreeNodes `freq` field
int found(TreeNode head, char symbol) {
    TreeNode curr = head;
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
int ordered(TreeNode head) {
    TreeNode curr = head;
    while(curr->parent != NULL) {
        if(curr->freq > curr->parent->freq) {
            return 0;
        }

        curr = curr->parent;
    }

    return 1;
}

void sort(TreeNode head) {
    if(head->parent == NULL) { return; }
    while(!ordered(head)) {
        TreeNode prev = head;
        TreeNode curr = head->parent;
        while(curr != NULL) {
            if(curr->freq < prev->freq) {
                // Copy TreeNode content
                char tempChar = prev->symbol;
                unsigned int tempFreq = prev->freq;
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

// Originally, my code just depended on the original file to decode,
// but I decided it should be independent
// This method writes the symbols and their frequencies to a file,
// separated by a null character
void writeFreqTable(char* file, TreeNode head) {
    // Build the output file name from the original file
    int len = strlen(file);
    char output[len + 5];
    strcpy(output, file);
    output[len + 0] = '.';
    output[len + 1] = 'f';
    output[len + 2] = 'r';
    output[len + 3] = 'e';
    output[len + 4] = 'q';
    output[len + 5] = '\0';

    FILE* fo;
    fo = fopen(output, "wb");

    TreeNode curr = head;
    while(curr != NULL) {
        fputc(curr->symbol, fo);
        fprintf(fo, "%u", curr->freq);
        fprintf(fo, "%c", (char) 0);

        curr = curr->parent;
    }

    fclose(fo);
}

// Builds a frequency table from a file produced by the above method
TreeNode readFreqTable(char* file) {
    FILE* fi;
    fi = fopen(file, "rb");

    // We need to keep track of the tail of the SLL
    // So we can add new TreeNodes w/o traversing the entire list
    TreeNode head = NULL;
    TreeNode tail = NULL;

    char tempChar;
    while(fscanf(fi, "%c", &tempChar) != EOF) {
        unsigned int tempFreq = 0;
        fscanf(fi, "%u", &tempFreq);

        // Construct the tree in reverse order to preserve ordering
        // without the need to sort
        TreeNode temp = newTreeNode(tempChar);
        temp->freq = tempFreq;
        if(head == NULL) {
            head = temp;
            tail = temp;
        } else {
            tail->parent = temp;
            tail = temp;
        }

        fscanf(fi, "%c", &tempFreq);
    }

    fclose(fi);

    return head;
}

int leavesCount(TreeNode head) {
    TreeNode curr = head;

    int count = 0;
    while(curr != NULL) { 
        count++;
        curr = curr->parent;
    }

    return count;
}

// Helper method for `leaves`
void leavesTraversal(TreeNode root, TreeNode** leaves, int* index) {
    if(root == NULL) { return; }
    
    if(root->left == NULL && root->right == NULL) {
        (*leaves)[*index] = root;
        (*index)++;
    } else {
        leavesTraversal(root->left, leaves, index);
        leavesTraversal(root->right, leaves, index);
    }
}

// Calls the recursive `leavesTraversal` method
// Which compiles a list of all leaf nodes on the huffman tree
TreeNode* leaves(TreeNode root, int leavesCount) {
    TreeNode* leaves = (TreeNode*) malloc (sizeof(TreeNode) * leavesCount);

    int index = 0;
    leavesTraversal(root, &leaves, &index);

    return leaves;
}

// The `parent` field on child nodes aren't maintained
// during the creation of the huffman tree
// This method repairs them
void links(TreeNode root) {
    if(root->left != NULL) {
        root->left->parent = root;
        links(root->left);
    }

    if(root->right != NULL) {
        root->right->parent = root;
        links(root->right);
    }
}

TreeNode createHuffmanTree(TreeNode head) {
    TreeNode curr = head;
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
        links(curr); // Repair child->parent links
    }

    // Returns the root of the newly-created Huffman tree
    return curr;
}

void encode(char* file, TreeNode** leaves, int leavesCount) {
    // Open the input file
    FILE* fi;
    fi = fopen(file, "rb");

    // Construct the output file's name
    int len = strlen(file);
    char output[len + 4];
    strcpy(output, file);
    output[len + 0] = '.';
    output[len + 1] = 'h';
    output[len + 2] = 'u';
    output[len + 3] = 'f';

    // Open output file
    FILE* fo;
    fo = fopen(output, "wb");

    len = 8;
    char buffer = 0;
    char temp;
    while(fscanf(fi, "%c", &temp) != EOF) {
        int i;
        for(i = 0; i < leavesCount; i++) {
            TreeNode curr = (*leaves)[i];
            if(curr->symbol == temp) {
                TreeNode temp = curr;
                int depth = 0;
                while(temp->parent != NULL) {
                    depth++;
                    temp = temp->parent;
                }

                // Originally, I used an unsigned long long as a buffer,
                // but the compression started to break down in files
                // with more than 64 different characters
                // This array acts as a single, arbitrarily-long buffer
                int walkSize = (depth % UL_SIZE) + 1;
                unsigned long walk[walkSize];
                memset(walk, 0, sizeof(unsigned long) * walkSize);

                // Walk the length of the tree until reaching the root
                int walkLen = 0;
                while(curr->parent != NULL) {
                    if(curr->parent->right == curr) { walk[0]++; }

                    // Advance the walk and shift the buffer by 1
                    walkLen++;
                    curr = curr->parent;
                    if(curr->parent != NULL) { 
                        // This whole block of code is just a left shift
                        // across the entire array
                        int MSB = 0;
                        int i;
                        for(i = 0; i < walkSize; i++) {
                            int temp = MSB;
                            // Preserve the bit so it can be added to 
                            // the following array element
                            MSB = (walk[i] >> (UL_SIZE - 1)) & 1;
                            walk[i] <<= 1;
                            walk[i] |= temp;
                        }
                    }
                }

                // Write data from the walk buffer to the write buffer
                while(walkLen > 0) {
                    // Read value from buffer then shift
                    int LSB = 0;
                    int i;
                    for(i = walkSize - 1; i >= 0; i--) {
                        int temp = LSB;
                        LSB = walk[i] & 1;
                        walk[i] >>= 1;
                        walk[i] |= temp;
                        
                    }

                    // Have to decrement the walk buffer size,
                    // since we took a bit from it
                    walkLen--;

                    // Add to write buffer
                    len--;
                    buffer |= (LSB << len);

                    // If buffer is full, write to file then clear it
                    if(len == 0) {
                        fputc(buffer, fo);
                        len = 8;
                        buffer = 0;
                    }
                }
            }
        }
    } 
    
    // Write any remaining bits to the end of the file
    // This results in a few extra bits at the end,
    // but seems unavoidable
    if(len != 8) { fputc(buffer, fo); }

    // Close file readers
    fclose(fi);
    fclose(fo);
}

void decode(char* file, TreeNode root) {
    // Input file
    FILE* fi;
    fi = fopen(file, "rb");

    // Build the output filename
    int len = strlen(file);
    char temp[len];
    strcpy(temp, file);
    temp[len - 3] = 'd';
    temp[len - 2] = 'e';
    temp[len - 1] = 'c';

    // Open the output file
    FILE* fo;
    fo = fopen(temp, "wb");
    
    char buffer;
    TreeNode curr = root;
    while(!feof(fi)) {
        // Can't check for EOF in loop condition.
        // Some trees result in a bit sequence that corresponds to 0x1A,
        // which is the DOS EOF code, which makes it fail on Windows
        fscanf(fi, "%c", &buffer);

        len = 8;
        while(len > 0) {
            // Write symbol and advance to next bit
            if(curr->left == NULL && curr->right == NULL) {
                fputc(curr->symbol, fo);
                curr = root;
            }

            // Advance down the tree in the appropriate direction
            int val = (buffer >> (len - 1)) & 1;
            curr = val ? curr->right : curr->left;

            // Represents the number of bits remaining
            // in the current buffer
            len--;
        }
    }

    // Close files
    fclose(fi);
    fclose(fo);
}
