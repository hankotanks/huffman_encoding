// Declare the struct here so huffman.c has access to its fields
struct node {
    int freq;
    char symbol;
    struct node* left;
    struct node* right;
    struct node* parent;
};

// The TreeNode type is actually a pointer
// This is because ALL struct node(s) will be created in heap
// So it's easier to keep track of (in my opinion)
typedef struct node * TreeNode;

void freeTree(TreeNode root);
void displayTree(TreeNode root);

// Initializes a new TreeNode on the heap
TreeNode newTreeNode(char symbol);
