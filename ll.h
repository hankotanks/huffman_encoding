struct node {
    int freq;
    char symbol;
    struct node* left;
    struct node* right;
    struct node* parent;
};

typedef struct node * TreeNode;

int isEmptyTree(TreeNode t);
void freeTree(TreeNode t);
void displayTree(TreeNode t);

TreeNode newTreeNode(char symbol);
