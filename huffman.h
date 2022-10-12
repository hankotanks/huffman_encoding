/* In the following method signatures:
 *   `head` refers to createFreqTable's output (a linked list of nodes)
 *   `root` refers to the top of the generated Huffman tree
 */

// Constuction, reading, and writing of frequency table 
TreeNode createFreqTable(char* file);
TreeNode readFreqTable(char* file);
void writeFreqTable(TreeNode head);

// Methods relating to the huffman tree
TreeNode createHuffmanTree(TreeNode head);
int leavesCount(TreeNode head);
TreeNode* leaves(TreeNode root, int leavesCount);

// Performing the compression/decompression
void encode(char* file, TreeNode** leaves, int leavesCount);
void decode(char* file, TreeNode root);

