
# Assignment - 4

## B+ Tree

#### HOW TO EXECUTE THE SCRIPT

1. Open terminal 

2. Run the command: "make"

3. a) Run the command:"./test_assign4" (For MAC and Linux), "test_assign4" (For Windows)

b) Run the command: "./test_expr" (For MAC and Linux), "test_expr" (For Windows)

4. To remove object files run the command "make clean"

Note: Change rm to del for make clean in make file for windows. 

## Overview of the Assignment:

### B-Tree Functions

- hmInit()
  - Initializes a new hash map.
  - Returns a pointer to the initialized hash map or NULL on failure.

- hash(int key)
  - Computes a hash for the given key using a bitwise shift-based function.
  - Returns the hash value as an integer.

- structHM(HashMap *hm, int key)
  - Retrieves the structure associated with a specific key in the hash map.
  - Returns a pointer to the hashmapStructure if the key is found, or NULL if not.

- insertHM(HashMap *hm, int key, void *val)
  - Inserts a new key-value pair into the hash map or updates the value if the key exists.
  - Returns 1 if inserted, 0 if updated, or -1 if an error occurred.

- getHM(HashMap *hm, int key)
  - Retrieves the value associated with a specified key from the hash map.
  - Returns the value associated with the key, or NULL if the key is not found.

- deleteHM(HashMap *hm, int key)
  - Deletes the key-value pair from the hash map based on the specified key.
  - Returns 1 if successfully deleted, or 0 if the key was not found.

- destroyHM(HashMap *hm)
  - Destroys the hash map and frees all associated resources.
  - This function does not return a value.

- dynamicArrSearch(dynamicArr *arr, int elem, int *fitOn)
  - Searches for an element in the dynamic array and determines its proper insertion point.
  - Returns the index of the element if found, or -1 if not found; fitOn is set to the index where it fits.

- initArr(int size)
  - Initializes a new dynamic array with the specified size.
  - Returns a pointer to the newly created dynamic array.

- destroyArr(dynamicArr *dynamicArray)
  - Frees the memory allocated for the dynamic array and its elements.
  - This function does not return a value.

- InsertAtArr(dynamicArr *dynamicArray, int element, int insertIndex)
  - Inserts an element at the specified index in the dynamic array.
  - Returns the index where the element was inserted, or -1 if the insertion failed.

- InsertArr(dynamicArr *dynamicArray, int elem)
  - Inserts an element into the dynamic array in a sorted manner.
  - Returns the index where the element was inserted or -1 if the array is full.

- DeleteAtArr(dynamicArr *dynamicArray, int startIndex, int deleteCount)
  - Deletes a segment of elements from the dynamic array starting at the specified index.
  - This function does not return a value.

- DeleteOneArr(dynamicArr *dynamicArray, int element)
  - Deletes a single instance of an element from the dynamic array.
  - Returns the index of the deleted element, or -1 if the element is not found.

- DeleteAllArr(dynamicArr *dynamicArray, int elem)
  - Deletes all occurrences of a specified element from the dynamic array.
  - Returns the index of the first deleted element, or -1 if the element is not found.

- emptyArr(dynamicArr *dynamicArray)
  - Empties all elements from the dynamic array.
  - This function does not return a value.

- freePointer(int count, ...)
  - Frees a list of pointers passed as variable arguments.
  - This function does not return a value.

- createBTNode(int size, int isLeaf, int pageNum)
  - Creates a new B-Tree node with specified characteristics.
  - Returns a pointer to the newly created BT_Node, or NULL on failure.

- destroyBTNode(BT_Node *node)
  - Frees the memory allocated for a B-Tree node and its associated structures.
  - This function does not return a value.

- printNodeHelper(BT_Node *node, char *result)
  - Formats the contents of a B-Tree node into a string.
  - Returns an RC status indicating success or failure of the operation.

- readNode(BT_Node **bTreeNode, BTreeHandle *tree, int pageNum)
  - Reads a B-Tree node from the disk using the specified page number.
  - Returns an RC status indicating success or failure of the operation.

- writeNode(BT_Node *node, BTreeHandle *tree)
  - Writes a B-Tree node to disk at the node's page number.
  - Returns an RC status indicating success or failure of the operation.

- printNode(BT_Node *node)
  - Prints detailed information about a B-Tree node to the console.
  - This function does not return a value.

- stack_init(Stack *stack)
  - Initializes a new stack to store B-Tree nodes.
  - This function does not return a value.

- stack_push(Stack *stack, BT_Node *data)
  - Pushes a B-Tree node onto the stack.
  - This function does not return a value.

- stack_pop(Stack *stack)
  - Pops the top B-Tree node from the stack.
  - Returns the popped B-Tree node, or NULL if the stack is empty.

- stack_empty(Stack *stack)
  - Checks if the stack is empty.
  - Returns 1 if the stack is empty, or 0 otherwise.

- stack_destroy(Stack *stack)
  - Destroys the stack and frees all associated resources.
  - This function does not return a value.

- loadBtreeNodes(BTreeHandle *tree, BT_Node *root, BT_Node **leftOnLevel, int level)
  - Loads B-Tree nodes into memory from disk starting from the specified root.
  - Returns an RC status indicating success or failure of the operation.

- findNodeByKey(BTreeHandle *tree, int key)
  - Finds the node containing a specified key in the B-Tree.
  - Returns the node containing the key, or NULL if the key is not found.

- initializeAndLoad(BTreeHandle *tree, BT_Node **leftOnLevel, RC *err)
  - Initializes and loads B-Tree nodes from disk.
  - This function does not return a value, but updates the error status through a pointer.

- loadBtree(BTreeHandle *tree)
  - Loads the entire B-Tree from disk into memory.
  - Returns an RC status indicating success or failure of the operation.

- writeBtreeHeader(BTreeHandle *tree)
  - Writes the header information of a B-Tree to disk.
  - Returns an RC status indicating success or failure of the operation.

- initIndexManager(void *mgmtData)
  - Initializes the index manager with the provided management data.
  - Returns an RC status indicating success or failure of the operation.

- insPropagateParent(BTreeHandle *tree, BT_Node *left, BT_Node *right, int key)
  - Inserts a key into the parent node during a split operation and manages the propagation of changes.
  - Returns an RC status indicating success or failure of the operation.

- freeNodes(BT_Node *root)
  - Frees all nodes of a B-Tree starting from the specified root.
  - This function does not return a value.

- createBtree(char *idxId, DataType keyType, int n)
  - Creates a new B-Tree and initializes the necessary metadata.
  - Returns an RC status indicating success or failure of the operation.

- printTree(BTreeHandle *tree)
  - Prints the entire B-Tree structure to the console.
  - Returns a string representation of the B-Tree.

- shutdownIndexManager()
  - Shuts down the index manager and performs necessary cleanup.
  - Returns an RC status indicating success or failure of the operation.

- openBtree(BTreeHandle **tree, char *idxId)
  - Opens an existing B-Tree from disk and initializes the B-Tree handle.
  - Returns an RC status indicating success or failure of the operation.

- closeBtree(BTreeHandle *tree)
  - Closes a B-Tree, releasing associated resources.
  - Returns an RC status indicating success or failure of the operation.

- deleteBtree(char *idxId)
  - Deletes a B-Tree file from the system.
  - Returns an RC status indicating success or failure of the operation.

- getNumNodes(BTreeHandle *tree, int *result)
  - Retrieves the number of nodes in the B-Tree.
  - Returns an RC status indicating success or failure of the operation.

- getNumEntries(BTreeHandle *tree, int *result)
  - Retrieves the number of entries (key-value pairs) in the B-Tree.
  - Returns an RC status indicating success or failure of the operation.

- getKeyType(BTreeHandle *tree, DataType *result)
  - Retrieves the data type of the keys in the B-Tree.
  - Returns an RC status indicating success or failure of the operation.

- findKey(BTreeHandle *tree, Value *key, RID *result)
  - Finds the key in the B-Tree and returns the corresponding Record ID (RID).
  - Returns an RC status indicating success or failure of the operation.

- closeTreeScan(BT_ScanHandle *handle)
  - Closes a B-Tree scan, releasing allocated memory.
  - Returns an RC status indicating success or failure of the operation.

- insertKey(BTreeHandle *tree, Value *key, RID rid)
  - Inserts a key-value pair into a B-Tree.
  - Returns an RC status indicating success or failure of the operation.

- deleteKey(BTreeHandle *tree, Value *key)
  - Deletes a key from the B-Tree, if present, and updates associated leaf page information.
  - Returns an RC status indicating success or failure of the operation.

- openTreeScan(BTreeHandle *tree, BT_ScanHandle **handle)
  - Opens a B-Tree scan for sequential access to all entries.
  - Returns an RC status indicating success or failure of the operation.

- nextEntry(BT_ScanHandle *handle, RID *result)
  - Advances the B-Tree scan to the next entry and retrieves the Record ID (RID).
  - Returns an RC status indicating success or failure of the operation.
