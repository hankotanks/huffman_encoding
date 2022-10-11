/* In the following method signatures:
 *   `head` refers to createFreqTable's output (a linked list of nodes)
 *   `root` refers to the top of the generated Huffman tree
 */

// Helper methods
TreeNode createFreqTable(char* file);
TreeNode createHuffmanTree(TreeNode head);
int leavesCount(TreeNode head);
TreeNode* leaves(TreeNode root, int leavesCount);

// Performing the compression/decompression
void encode(char* file, TreeNode** leaves, int leavesCount);
void decode(char* file, TreeNode root);

