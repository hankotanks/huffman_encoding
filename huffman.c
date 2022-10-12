#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "tree.h"
#include "huffman.h"

int main(int argc, char* argv[]) {
    // Make sure the right number of arguments are present
    if(argc < 3 || argc > 4) { 
        printf("Error: Incorrect argument format\n"); 
        exit(1);
    }
    
    // Check if the user wants to encode or decode
    int mode;
    if(strcmp(argv[1], "-e") == 0) {
        mode = 1;
    } else if(strcmp(argv[1], "-d") == 0) { 
        mode = 0;
        if(argc != 4) {
            printf("Error: Incorrect argument format\n");
            exit(1);
        }
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

        writeFreqTable(head);

        TreeNode t = head;
        while(t->parent != NULL) {
            printf("%c : %d\n", t->symbol, t->freq);
            t = t->parent;
        }

        // Build an array of leaf nodes
        int leafCount = leavesCount(head);
        
        // Construct the huffman tree
        root = createHuffmanTree(head);
        TreeNode* leafNodes = leaves(root, leafCount);
        
        encode(argv[2], &leafNodes, leafCount);
    } else {
        // Confirm that the file has the proper extension
        
        int len = strlen(argv[2]);
        const char* ext = &(argv[2])[len - 4];
        const char* huf = ".huf";
        if(strcmp(ext, huf) != 0) {
            printf("Error: Invalid file extension (should be *.huf)");
            exit(1);
        }

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

void writeFreqTable(TreeNode head) {
    FILE* fo;
    fo = fopen("freq", "w");

    TreeNode curr = head;
    while(curr != NULL) {
        fputc(curr->symbol, fo);
        fprintf(fo, "%u\n", curr->freq);

        curr = curr->parent;
    }

    fclose(fo);
}

TreeNode readFreqTable(char* file) {
    FILE* fi;
    fi = fopen(file, "rb");
    if(fi == NULL) {
        printf("Error: Couldn't open the given freq table\n");
        exit(1);
    }

    TreeNode head = NULL;

    char tempChar;
    while(fscanf(fi, "%c", &tempChar) != EOF) {
        unsigned int tempFreq = 0;
        fscanf(fi, "%u\n", &tempFreq);

        TreeNode temp = newTreeNode(tempChar);
        temp->freq = tempFreq;
        temp->parent = head;
        
        head = temp;
    }

    fclose(fi);

    sort(head);

    TreeNode t = head;
    while(t != NULL) {
        printf("%c : %d\n", t->symbol, t->freq);
        t = t->parent;
    }

    return head;

}

void encode(char* file, TreeNode** leaves, int leavesCount) {
    // Open the input file
    FILE* fi;
    fi = fopen(file, "r");
    if(fi == NULL) { // If file doesn't exist
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

    // Open output file
    FILE* fo;
    fo = fopen(output, "w");

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

                int walkSize = (depth % sizeof(unsigned long)) + 1;
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
                        int MSB = walk[0] >> (sizeof(unsigned long) - 1);
                        walk[0] <<= 1;
                        int i;
                        for(i = 1; i < walkSize; i++) {
                            int temp = MSB;
                            MSB = walk[i] >> (sizeof(unsigned long) - 1);
                            walk[i] <<= 1;
                            walk[i] |= temp;
                        }
                    }
                }

                // Write data from the walk buffer to the write buffer
                while(walkLen > 0) {
                    // Read value from buffer then shift
                    int val = (walk[0] & 1);

                    walk[0] >>= 1;
                    int i;
                    for(i = 1; i < walkSize; i++) {
                        walk[i - 1] |= (walk[i] & 1) << (sizeof(unsigned long) - 1); 
                        walk[i] >>= 1;
                    }

                    walkLen--;

                    // Add to write buffer
                    len--;
                    buffer |= (val << len);

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
    fo = fopen(temp, "w");
    
    char buffer;
    TreeNode curr = root;
    while(!feof(fi)) { // CANNOT CHECK FSCANF STATUS AS BREAK CONDITION, encoded file can contain EOF prematurely
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
