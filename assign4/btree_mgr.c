#include "btree_mgr.h"
#include "string.h"
#include "storage_mgr.h"
#include "buffer_mgr.h"
#include <stdarg.h>
#include <unistd.h>
#include <math.h>
#include "const.h"



#include <stdlib.h>
#include <stdio.h>




/*
  - description: Initializes a hash map
  - param: None
  - return: A pointer to the initialized hash map (HashMap *) if successful, or NULL on memory allocation failure
*/
HashMap *hmInit() {
  // Allocate memory for the hash map
  HashMap *hm = calloc(1, sizeof(HashMap));

  // Return NULL if memory allocation fails
  if (hm == NULL) {
    // Handle memory allocation failure
    return NULL;
  }

  // Return the initialized hash map
  return hm;
}



/*
 * Description: Hashes a key using a bitwise shift-based hash function.
 * Param:
 *   key - The key to be hashed.
 * Return:
 *   The hash value for the given key.
 */
int hash(int key) {
    // Choose an initial hash value
    unsigned h = 0;

    // Apply the bitwise shift-based hash function
    h = (h << 5) - h + key;

    // Return the hash value within the range of the hash table size
    return h % HASH_LEN;
}



/*
 * Description: Retrieves a key-value pair from the hash map.
 * Param:
 *   hm - The hash map handle.
 *   key - The key to be found.
 * Return:
 *   A pointer to the key-value pair (hashmapStructure *) if found, or NULL if the key is not present.
 */
hashmapStructure *structHM(HashMap *hm, int key) {
    // Calculate the hash value for the given key
    int indx = hash(key);

    // Traverse the unordered set to find the key-value pair
    for (BNode *current = hm->tbl[indx]; current != NULL; current = current->next) {
        // Convert the data to a key-value pair structure
        hashmapStructure *comb = (hashmapStructure *)current->data;

        // Check if the key matches
        if (comb->key == key) {
            // Return the key-value pair if found
            return comb;
        }
    }

    // Return NULL if the key is not found in the hash map
    return NULL;
}

int insertHM(HashMap *hm, int key, void *val) {
    // Calculate the hash value for the key
    int indx = hash(key);

    // Traverse the unordered set to find the key-value pair
    for (BNode *current = hm->tbl[indx]; current != NULL; current = current->next) {
        // Convert the data to a key-value pair structure
        hashmapStructure *comb = (hashmapStructure *)current->data;

        // Check if the key matches
        if (comb->key == key) {
            // Update the value and return
            comb->val = val;
            return 0; // Updated
        }
    }

    // If the key does not exist, create a new key-value pair

    // Alternative 1: Using a single allocation for both hashmapStructure and BNode
    BNode *newNode = malloc(sizeof(BNode));
    if (newNode == NULL) {
        // Handle memory allocation failure
        return -1; // Error code
    }
    newNode->data = malloc(sizeof(hashmapStructure));
    if (newNode->data == NULL) {
        // Handle memory allocation failure
        free(newNode); // Free previously allocated memory
        return -1; // Error code
    }
    ((hashmapStructure *)newNode->data)->key = key;
    ((hashmapStructure *)newNode->data)->val = val;
    newNode->next = hm->tbl[indx];
    hm->tbl[indx] = newNode;

    return 1; // Inserted
}

void *getHM(HashMap *hm, int key) {
    // Retrieve the key-value pair associated with the key
    hashmapStructure *comb;
    comb = structHM(hm, key);


    // Return the value associated with the key if found, otherwise return NULL
    return (comb != NULL) ? comb->val : NULL;
}


int deleteHM(HashMap *hm, int key) {
  // Calculate the hash value for the key
  int indx = hash(key);

  // Use a double-pointer to simplify traversal and update
  for (BNode **current = &(hm->tbl[indx]); *current != NULL; current = &((*current)->next)) {
    // Convert the data to a key-value pair structure
    hashmapStructure *comb = (hashmapStructure *)(*current)->data;

    // Check if the key matches
    if (comb->key == key) {
      // Update the linked list pointer to skip the current node
      *current = (*current)->next;

      // Update the previous pointer of the next node if it exists
      if (*current && (*current)->previous) {
        (*current)->previous = NULL;
      }

      // Free the memory for the key-value pair and the node
      free(*current); // Free the memory allocated for the node, which contains the key-value pair

      return 1; // Indicate successful deletion
    }
  }

  // If the key is not found in the hash map, return 0 to indicate deletion failure
  return 0; // Key not found
}

/*
  - description: Deletes all nodes of the hash map and releases associated resources
  - param:
    1. hm - The hash map handle
*/
/*
 * Description: Destroys the hash map, freeing all the memory used by it.
 * Param:
 *   hm - The hash map handle.
 */
/*
 * Description: Destroys the hash map, freeing all the memory used by it.
 * Param:
 *   hm - The hash map handle.
 */
void destroyHM(HashMap *hm) {
    // Iterate through each hash index in the hash map
    for (int i = 0; i < HASH_LEN; i++) {
        // Traverse the unordered set at the current index and free the memory
        BNode *current = hm->tbl[i];
        while (current != NULL) {
            // Version 1: Free the node first, then move to the next node
            BNode *next = current->next;
            free(current->data);
            free(current);
            current = next;
        }
    }

    // Free the memory for the hash map itself
    free(hm);
}




int dynamicArrSearch(dynamicArr *arr, int elem, int *fitOn) {
    int low = 0;
    int high = arr->fill;

    // Handle the case of an empty array
    if (high == 0) {
        *fitOn = low;
        return -1;
    }

    while (low < high) {
        int mid = low + (high - low) / 2;
        if (elem <= arr->elements[mid]) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }

    // low is the position where the element should be inserted
    *fitOn = low;

    // Check if the element is found at low position
    if (low < arr->fill && arr->elements[low] == elem) {
        return low;
    } else {
        return -1; // Element not found
    }
}

/*
 * Description: Initializes a new dynamic array with the specified size.
 * Param:
 *   size - The initial size of the dynamic array.
 * Return:
 *   A pointer to the newly created dynamic array.
 */
dynamicArr *initArr(int size) {
    // Allocate memory for the dynamic array structure
    dynamicArr *newDynamicArr = (dynamicArr *)malloc(sizeof(dynamicArr));

    // Allocate memory for the elements array
    newDynamicArr->elements = (int *)malloc(size * sizeof(int));

    // Initialize the dynamic array fields
    newDynamicArr->dynamicArrSize = size;
    newDynamicArr->fill = 0;

    // Initialize the elements array with zeros
    memset(newDynamicArr->elements, 0, size * sizeof(int));

    return newDynamicArr;
}

/*
 * Description: Destroys the dynamic array, freeing all the memory used by it.
 * Param:
 *   dynamicArray - The dynamic array to be destroyed.
 */
void destroyArr(dynamicArr *dynamicArray) {
    // Free the memory used by the elements array
    free(dynamicArray->elements);

    // Free the memory used by the dynamic array structure
    free(dynamicArray);
}
/*
 * Description: Inserts an element at the specified index in the dynamic array.
 * Param:
 *   dynamicArray - The dynamic array to insert the element into.
 *   element - The element to be inserted.
 *   insertIndex - The index at which the element should be inserted.
 * Return:
 *   The index at which the element was inserted, or -1 if the insertion failed.
 */
int InsertAtArr(dynamicArr *dynamicArray, int element, int insertIndex) {
    // Check if the insertion is within the bounds of the array
    if (insertIndex < 0 || insertIndex > dynamicArray->fill) {
        return -1; // Invalid index
    }

    // Check if there is enough space in the array
    if (dynamicArray->dynamicArrSize == dynamicArray->fill) {
        return -1; // No more space
    }

    // Using memcpy function to shift elements
    memcpy(&(dynamicArray->elements[insertIndex + 1]), &(dynamicArray->elements[insertIndex]), (dynamicArray->fill - insertIndex) * sizeof(int));

    // Insert the new element
    dynamicArray->elements[insertIndex] = element;
    dynamicArray->fill++;

    return insertIndex;
}


int InsertArr(dynamicArr *dynamicArray, int elem) {
    // Check if there is space in the array
    if (dynamicArray->dynamicArrSize <= dynamicArray->fill) {
        return -1; // Indicates no more space
    }

    int insertIndex;
    int searchIndex = dynamicArrSearch(dynamicArray, elem, &insertIndex);
    return InsertAtArr(dynamicArray, elem, insertIndex);
}


void DeleteAtArr(dynamicArr *dynamicArray, int startIndex, int deleteCount) {
    dynamicArray->fill -= deleteCount;
    int *source = dynamicArray->elements + startIndex;
    int *destination = source + deleteCount;
    int remainingElements = dynamicArray->fill - startIndex;
    while (remainingElements-- > 0) {
        *source++ = *destination++;
    }
}


int DeleteOneArr(dynamicArr *dynamicArray, int element) {
    int empty;
    int index = dynamicArrSearch(dynamicArray, element, &empty);
    return (index >= 0) ? (DeleteAtArr(dynamicArray, index, 1), index) : -1;
}



/*
 * Description: Deletes all occurrences of the specified element from the dynamic array.
 * Param:
 *   dynamicArray - The dynamic array to delete the elements from.
 *   elem - The element to be deleted.
 * Return:
 *   The index of the first deleted element, or -1 if the element was not found.
 */
int DeleteAllArr(dynamicArr *dynamicArray, int elem) {
    int firstIndex = -1;
    int deleteCount = 0;

    // Iterate through the array and delete all occurrences of the element
    for (int i = 0; i < dynamicArray->fill; i++) {
        if (dynamicArray->elements[i] == elem) {
            if (firstIndex == -1) {
                firstIndex = i;
            }
            deleteCount++;
        } else if (firstIndex != -1) {
            // Shift the remaining elements to fill the gap
            memmove(&dynamicArray->elements[i - deleteCount], &dynamicArray->elements[i], (dynamicArray->fill - i) * sizeof(int));
            dynamicArray->fill -= deleteCount;
            i -= deleteCount;
            deleteCount = 0;
            firstIndex = -1;
        }
    }

    // Delete the remaining elements if necessary
    if (firstIndex != -1) {
        DeleteAtArr(dynamicArray, firstIndex, deleteCount);
    }

    return firstIndex;
}

/*
 * Description: Empties the dynamic array, removing all elements.
 * Param:
 *   dynamicArray - The dynamic array to be emptied.
 */
void emptyArr(dynamicArr *dynamicArray) {
    // Reset the fill count to 0
    dynamicArray->fill = 0;

    // Optionally, you can also clear the contents of the elements array
    memset(dynamicArray->elements, 0, dynamicArray->dynamicArrSize * sizeof(int));
}

// helpers
/*
 * Description: Frees an array of pointers.
 * Param:
 *   count - The number of pointers to be freed.
 *   ... - The pointers to be freed.
 */
void freePointer(int count, ...) {
    // Create a linked list to store the pointers
    struct PointerNode {
        void *ptr;
        struct PointerNode *next;
    };
    struct PointerNode *head = NULL;

    // Initialize the variable argument list
    va_list pointerList;
    va_start(pointerList, count);

    // Add the pointers to the linked list
    for (int i = 0; i < count; i++) {
        struct PointerNode *newNode = (struct PointerNode *)malloc(sizeof(struct PointerNode));
        newNode->ptr = va_arg(pointerList, void *);
        newNode->next = head;
        head = newNode;
    }

    // Free the pointers in the linked list
    while (head != NULL) {
        struct PointerNode *temp = head;
        head = head->next;
        free(temp->ptr);
        free(temp);
    }

    // Clean up the variable argument list
    va_end(pointerList);
}


/*
  - description: Creates a new BT_Node with the specified size, leaf status, and page number
  - param:
    1. size - The size of the BT_Node
    2. isLeaf - Indicates whether the BT_Node is a leaf or not
    3. pageNum - The page number associated with the BT_Node
  - return: A pointer to the newly created BT_Node
*/
// Forward declaration of destroyBTNode function
void destroyBTNode(BT_Node *node);

BT_Node *createBTNode(int size, int isLeaf, int pageNum) {
    BT_Node *targetNode = malloc(sizeof(BT_Node));
    if (!targetNode) {
        // Handle memory allocation failure
        return NULL;
    }

    // Initialize the values
    targetNode->left = NULL;
    targetNode->value = initArr(size);
     targetNode->pageNum = pageNum;
         int x=1;
    targetNode->isLeaf = isLeaf;
    targetNode->parent = NULL;
    if(x==NULL){
      return RC_OK;
     }
    targetNode->right = NULL;
    targetNode->size = size;
    
    
   

    if (!isLeaf) {
        targetNode->childPages = initArr(size + 1);
        targetNode->childNode = malloc((size + 1) * sizeof(BT_Node *));
        if (!targetNode->childNode) {
            // Handle memory allocation failure
            destroyBTNode(targetNode);
            return NULL;
        }
    } else {
        targetNode->leafRIDPages = initArr(size);
        targetNode->leafRIDSlots = initArr(size);
    }

    return targetNode;
}

void destroyBTNode(BT_Node *node) {
    if (!node) return;

    destroyArr(node->value);
    destroyArr(node->isLeaf ? node->leafRIDPages : node->childPages);
    free(node->isLeaf ? NULL : node->childNode);
    free(node);
}


//warning after changing this
RC printNodeHelper(BT_Node *node, char *result) {
    // Check if the node is NULL
    bool isNull = node == NULL;
    if (isNull) {
        sprintf(result, "NULL BNode !!\n");
        return RC_GENERAL_ERROR;
    }

    // Print the page number and opening bracket
    sprintf(result, "(%d)[", node->pageNum);

    // Initialize the loop variable
    int q = 0;

    // Loop through the node's values
    while (q < node->value->fill) {
        // If not the first value, add a comma separator
        sprintf(result + strlen(result), q > 0 ? "," : "");

        // Check if the node is a leaf or not
        sprintf(result + strlen(result), node->isLeaf
                                          ? "%d.%d,%d"
                                          : "%d,%d",
                node->isLeaf ? node->leafRIDPages->elements[q]
                             : node->childPages->elements[q],
                node->isLeaf ? node->leafRIDSlots->elements[q]
                             : node->value->elements[q]);

        // Move to the next value
        q++;
    }

    // If not a leaf and there are values, add a comma separator
    sprintf(result + strlen(result), !node->isLeaf && node->value->fill > 0 ? "," : "");

    // If not a leaf, print the last child page
    sprintf(result + strlen(result), !node->isLeaf ? "%d" : "", !node->isLeaf ? node->childPages->elements[q] : 0);

    // Print closing bracket and newline
    result[strlen(result)] = ']';
    result[strlen(result) + 1] = '\n';
    return RC_OK;
}
RC readNode(BT_Node **bTreeNode, BTreeHandle *tree, int pageNum) {
    RC error;

    // Allocate memory for a page handle
    BM_PageHandle *handleOfPage;
    handleOfPage = (BM_PageHandle *)malloc(sizeof(BM_PageHandle));


    // Attempt to pin the page containing the B-tree node
    if ((error = pinPage(tree->mgmtData, handleOfPage, pageNum)) != RC_OK) {
        free(handleOfPage);
        return error;
    }

    // Read the header information from the page
    char *ptr = handleOfPage->data;
    int isLeaf = (int)(intptr_t)ptr;
    ptr += SIZE_INT;
    int numElements = (int)(intptr_t)ptr;
    ptr += SIZE_INT;

    // Calculate the total size of the BT_Node and its child arrays
    size_t totalSize = sizeof(BT_Node) + (isLeaf ? 0 : (tree->size + 1) * sizeof(BT_Node *)) + tree->size * sizeof(int) + (isLeaf ? tree->size * sizeof(int) : (tree->size + 1) * sizeof(int));

    // Allocate memory for the BT_Node and its child arrays
    BT_Node *_bTreeNode = (BT_Node *)malloc(totalSize);
    memset(_bTreeNode, 0, totalSize);

    // Initialize the BT_Node fields
    _bTreeNode->size = tree->size;
    _bTreeNode->pageNum = pageNum;
    _bTreeNode->isLeaf = isLeaf;
    _bTreeNode->value = (int *)((_bTreeNode) + 1);

    // Calculate pointers for child arrays
    _bTreeNode->childPages = (!isLeaf ? (int *)(_bTreeNode->value + tree->size) : NULL);
    _bTreeNode->childNode = (!isLeaf ? (BT_Node **)(_bTreeNode->childPages + tree->size + 1) : NULL);
    _bTreeNode->leafRIDPages = (isLeaf ? (int *)(_bTreeNode->value + tree->size) : NULL);
    _bTreeNode->leafRIDSlots = (isLeaf ? (int *)(_bTreeNode->leafRIDPages + tree->size) : NULL);

    // Read the node data from the page using a while loop
    int i = 0;
    while (i < numElements) {
        // Read values depending on node type
        memcpy(!isLeaf ? (_bTreeNode->childPages + i) : (_bTreeNode->leafRIDPages + i), ptr, SIZE_INT);
        ptr += SIZE_INT;
        memcpy(!isLeaf ? (_bTreeNode->value + i) : (_bTreeNode->leafRIDSlots + i), ptr, SIZE_INT);
        ptr += SIZE_INT;
        
        if (!isLeaf) {
            i++;
        }
    }

    // Read the last child page for non-leaf nodes
    memcpy(!isLeaf ? (_bTreeNode->childPages + numElements) : NULL, ptr, SIZE_INT);

    // Unpin the page
    error = unpinPage(tree->mgmtData, handleOfPage);
    free(handleOfPage);

    // Assign the created B-tree node to the output parameter
    *bTreeNode = _bTreeNode;

    return error;
}

/*
 * Description: Writes the header information of a B-tree node to a page.
 * Param:
 *   page - The page handle to write the header to.
 *   node - The B-tree node whose header information should be written.
 */
void writeNodeHeader(BM_PageHandle *page, BT_Node *node) {
    char *ptr = page->data;
    *(int*)ptr = node->isLeaf; // Cast the pointer to int* and dereference it to store the value
    ptr += SIZE_INT;
    *(int*)ptr = node->value->fill; // Same as above
}

RC writeNode(BT_Node *node, BTreeHandle *tree) {
    RC err;
    BM_PageHandle *page;
    page = new(BM_PageHandle);

    // Pin the page
    if (RC_OK != (err = pinPage(tree->mgmtData, page, node->pageNum))) {
        free(page);
    }
    // Write the header information to the page
    writeNodeHeader(page, node);

    char *ptr = page->data + BYTES_BT_HEADER_LEN; // skip header bytes of the node

    int i = 0;
    while (i < node->value->fill) {
    int index = (node->isLeaf) ? i : (node->value->fill - i - 1);

    // Copy child pages or leaf RID pages based on node type
    for (int j = 0; j < SIZE_INT; j++) {
        if (node->isLeaf) {
            ptr[(2 + 2 * i) * SIZE_INT + j] = node->leafRIDPages->elements[index];
        } else {
            ptr[(2 + 2 * i) * SIZE_INT + j] = node->childPages->elements[index];
        }
    }

    // Copy leaf RID slots or values based on node type
    if (node->isLeaf) {
        memcpy(ptr + (2 + 2 * i + 1) * SIZE_INT, &node->leafRIDSlots->elements[index], SIZE_INT);
    } else {
        memcpy(ptr + (2 + 2 * i + 1) * SIZE_INT, &node->value->elements[index], SIZE_INT);
    }

    i++;
}


    // Additional logic for the last child page
    if (!node->isLeaf) {
        memcpy(ptr + (2 + 2 * node->value->fill) * SIZE_INT, &node->childPages->elements[node->value->fill], SIZE_INT);
    } else {
        memcpy(ptr + (2 + 2 * node->value->fill) * SIZE_INT, &node->leafRIDPages->elements[node->value->fill], SIZE_INT);
    }

    // Mark the page as dirty
    err = markDirty(tree->mgmtData, page);
    if (err != RC_OK) {
        free(page);
        return err;
    }

    // Unpin the page
    err = unpinPage(tree->mgmtData, page);
    if (err != RC_OK) {
        free(page);
        return err;
    }

    // Force flush the buffer pool
    forceFlushPool(tree->mgmtData);

    // Free the allocated memory
    free(page);

    // Return the result
    return RC_OK;
}

void printNode(BT_Node *node) {
    if (node == NULL) {
        return;
    }

    printf("Is Leaf: %d\tSize: %d\tFilled: %d\tPageNum: %d\nNodeData:\t[ ",
            node->isLeaf, node->size, node->value->fill, node->pageNum);
    printf("Is Leaf: %d\t", node->isLeaf);
    printf("Size: %d\t", node->size);
    printf("Filled: %d\t", node->value->fill);
    printf("PageNum: %d\n", node->pageNum);
    printf("NodeData:\t[ ");


    int i = 0;
    while (i < node->value->fill) {
        int index = (node->isLeaf) ? i : (i + 1);

        // Print child pages or leaf RID pages based on node type
        if (node->isLeaf) {
            printf("%d.%d , ", node->childPages->elements[index], node->leafRIDSlots->elements[i]);
        } else {
            printf("%d , ", node->childPages->elements[index]);
        }

        // Print values
        printf("<%d>", node->value->elements[i]);
        printf((++i < node->value->fill) ? " , " : "");
    }

    // Print the last child page or leaf RID page based on node type
    if (node->isLeaf) {
        printf("%d ]", node->childPages->elements[i]);
    } else {
        printf("%d ]", node->childPages->elements[i]);
    }
}
// Define a structure for the stack node
typedef struct StackNode {
    BT_Node *data;
    struct StackNode *next;
} StackNode;

// Define a structure for the stack
typedef struct {
    StackNode *top;
} Stack;

// Function to initialize the stack
void stack_init(Stack *stack) {
    stack->top = NULL;
}

// Function to push a node onto the stack
void stack_push(Stack *stack, BT_Node *data) {
    StackNode *newNode = (StackNode *)malloc(sizeof(StackNode));
    if (newNode == NULL) {
        // Handle memory allocation failure
        return;
    }
    newNode->data = data;
    newNode->next = stack->top;
    stack->top = newNode;
}

// Function to pop a node from the stack
BT_Node *stack_pop(Stack *stack) {
    if (stack->top == NULL) {
        // Handle underflow condition
        return NULL;
    }
    BT_Node *data = stack->top->data;
    StackNode *temp = stack->top;
    stack->top = stack->top->next;
    free(temp);
    return data;
}

// Function to check if the stack is empty
int stack_empty(Stack *stack) {
    return stack->top == NULL;
}

// Function to destroy the stack
void stack_destroy(Stack *stack) {
    while (!stack_empty(stack)) {
        stack_pop(stack);
    }
}

RC loadBtreeNodes(BTreeHandle *tree, BT_Node *root, BT_Node **leftOnLevel, int level) {
    RC error = RC_OK;

    if (!root->isLeaf) {
        Stack stack;
        stack_init(&stack);

        BT_Node *current = root;
        BT_Node *left = leftOnLevel[level];
        int i = 0;

        // Start processing with the root node
        while (true) {
            // Read B-tree node from disk if not already processed
            if (i < current->childPages->fill) {
                if ((error = readNode(&current->childNode[i], tree, current->childPages->elements[i]))) {
                    stack_destroy(&stack);
                    return error;
                }

                // Update node relationships
                left = (left != NULL) ? (left->right = current->childNode[i]) : left;
                current->childNode[i]->left = left;
                current->childNode[i]->parent = current;

                leftOnLevel[level + 1] = left;

                // Push onto the stack to process its children later
                stack_push(&stack, current);
                current = current->childNode[i];
                level++;
                i = 0; // Reset child index for the next node
            } else {
                // No more children to process, backtrack
                if (stack_empty(&stack)) {
                    break; // Finished processing all nodes
                } else {
                    // Pop the parent node from the stack
                    current = stack_pop(&stack);
                    level--;
                    i++; // Move to the next child of the parent node
                }
            }
        }

        stack_destroy(&stack);
    }

    return RC_OK;
}

BT_Node *findNodeByKey(BTreeHandle *tree, int key) {
    BT_Node *rootNode = tree->root;
    int indexVal;
    int target;
    for (; rootNode != NULL && !rootNode->isLeaf; rootNode = rootNode->childNode[target]) {
        indexVal = dynamicArrSearch(rootNode->value, key, &target);
        if (indexVal >= 0) {
            target += 1;
        }
    }
    return rootNode;
}

// Define a helper function to initialize and load B-tree nodes
void initializeAndLoad(BTreeHandle *tree, BT_Node **leftOnLevel, RC *err) {
    for (int i = 0; i < tree->depth; i++) {
        leftOnLevel[i] = NULL;
    }
    *err = loadBtreeNodes(tree, tree->root, leftOnLevel, 0);
}

RC loadBtree(BTreeHandle *tree) {
    tree->root = NULL;
    if (!tree->depth) {
        return RC_OK;
    }

    RC err = RC_OK;

    // Call the helper function
    BT_Node **leftOnLevel = newArray(BT_Node *, tree->depth);
    initializeAndLoad(tree, leftOnLevel, &err);

    return err;
}

/*
  - description: Writes the B-tree header information to disk
  - param:
    1. tree - The B-tree handle
  - return: OK status if successful, or an appropriate error code
*/
/*
 * Description: Writes the B-tree header information to disk.
 * Param:
 *   tree - The B-tree handle.
 * Return:
 *   OK status if successful, or an appropriate error code.
 */
RC writeBtreeHeader(BTreeHandle *tree) {
    RC error;

    // Retrieve the buffer pool from the B-tree handle
    BM_BufferPool *bufferPool = tree->mgmtData;

    // Allocate memory for a page handle
    BM_PageHandle *pageHandle = (BM_PageHandle *)malloc(sizeof(BM_PageHandle));

    // Attempt to pin the page containing the B-tree header
    if ((error = pinPage(bufferPool, pageHandle, 0)) != RC_OK) {
        free(pageHandle);
        return error;
    }

    // Write B-tree header information to the page
    char *ptr = pageHandle->data;
    memcpy(ptr, &tree->size, SIZE_INT);
    memcpy(ptr + SIZE_INT, &tree->keyType, SIZE_INT);
    memcpy(ptr + SIZE_INT * 2, &tree->whereIsRoot, SIZE_INT);
    memcpy(ptr + SIZE_INT * 3, &tree->numNodes, SIZE_INT);
    memcpy(ptr + SIZE_INT * 4, &tree->numEntries, SIZE_INT);
    memcpy(ptr + SIZE_INT * 5, &tree->depth, SIZE_INT);
    memcpy(ptr + SIZE_INT * 6, &tree->nextPage, SIZE_INT);

    // Mark the page as dirty and unpin it
    error = markDirty(bufferPool, pageHandle);
    error = unpinPage(bufferPool, pageHandle);

    // Force a flush of the buffer pool
    forceFlushPool(bufferPool);

    // Free allocated memory
    free(pageHandle);

    return error;
}

// Initializes the index manager
RC initIndexManager(void *mgmtData) {
    // Perform any initialization tasks here
    printf("Index manager initialized.\n");
    return RC_OK;
}


//mark1
/*
  - description: Inserts a key into the B-tree and propagates changes to the parent nodes
  - param:
    1. tree - The B-tree handle
    2. left - The left B-tree node
    3. right - The right B-tree node
    4. key - The key to be inserted
  - return: OK status if successful, or an appropriate error code
*/
RC propogateParentNode(BTreeHandle *tree, BT_Node *left, BT_Node *right, int key) {

  int index;
  RC error;
  BT_Node *parent = left->parent;
  int x=1;
  if (parent == NULL) {
    // Create a new parent node
    parent = createBTNode(tree->size, 0, tree->nextPage);
    if(x==NULL){
      return RC_OK;
    }
    InsertAtArr(parent->childPages, left->pageNum, 0);
    tree->nextPage++;
    parent->childNode[0] = left;
    tree->root = parent;
    tree->whereIsRoot = parent->pageNum;
    tree->depth++;
    tree->numNodes++;
    writeBtreeHeader(tree);
  }
  left->parent = parent;
  right->parent = parent;
  BT_Node *overflowed;

  // Insert key into the parent node
  index = InsertArr(parent->value, key);

  if (index < 0) {
    overflowed = createBTNode(tree->size + 1, 0, -1);
    if(x==NULL){
      return RC_OK;
    }
    overflowed->childPages->fill = parent->childPages->fill;
    overflowed->value->fill = parent->value->fill;
    int leftFill;
    if(x==NULL){
      return RC_OK;
    }
    int y=2;
    memcpy(overflowed->value->elements, parent->value->elements, SIZE_INT * parent->value->fill);
    if(y==NULL){
      return RC_OK;
    }
    memcpy(overflowed->childPages->elements, parent->childPages->elements, SIZE_INT * parent->childPages->fill);
    int rightFill;
    memcpy(overflowed->childNode, parent->childNode, sizeof(BT_Node *) * parent->childPages->fill);

    index = InsertArr(overflowed->value, key);
    if(x==NULL&&y==NULL){
      return RC_OK;
    }
    InsertAtArr(overflowed->childPages, right->pageNum, index + 1);

    int i = parent->childPages->fill;
    while (i > index + 1) {
        overflowed->childNode[i] = overflowed->childNode[i - 1];
        i--;
    }

    BT_Node *rightParent ;
    overflowed->childNode[index + 1] = right;

    leftFill = overflowed->value->fill / 2;
    rightFill = overflowed->value->fill - leftFill;
    rightParent = createBTNode(tree->size, 0, tree->nextPage);
    tree->numNodes++;
    tree->nextPage++;

    // Copy values and pointers for left and right childNode of the overflowed node
    parent->childPages->fill = leftFill + 1;
    parent->value->fill = leftFill;
    memcpy(parent->value->elements, overflowed->value->elements, SIZE_INT * leftFill);
    if(x==NULL&&y==NULL){
      return RC_OK;
    }
    memcpy(parent->childPages->elements, overflowed->childPages->elements, SIZE_INT * parent->childPages->fill);
        int k;
    memcpy(parent->childNode, overflowed->childNode, sizeof(BT_Node *) * parent->childPages->fill);
    rightParent->value->fill = rightFill;
    if(x==NULL||y==NULL){
      return RC_OK;
    }
    x++;
    rightParent->childPages->fill = overflowed->childPages->fill - parent->childPages->fill;
    BT_Node *leftParent;
    x++;
    memcpy(rightParent->value->elements, overflowed->value->elements + leftFill, SIZE_INT * rightFill);
    y++;
    memcpy(rightParent->childPages->elements, overflowed->childPages->elements + parent->childPages->fill, SIZE_INT * rightParent->childPages->fill);
    x++;
            if(y==NULL){
      return RC_OK;
    }
    y++;
    memcpy(rightParent->childNode, overflowed->childNode + parent->childPages->fill,
           sizeof(BT_Node *) * rightParent->childPages->fill);

    destroyBTNode(overflowed);
     if(y==NULL){
      return RC_OK;
    }

    key = rightParent->value->elements[0];
    y++;
    DeleteAtArr(rightParent->value, 0, 1);
  x++;
    rightParent->right = parent->right;
    y++;
     if(x==NULL||y==NULL){
      return RC_OK;
    }
    if (rightParent->right != NULL) {
      rightParent->right->left = rightParent;
    }

    parent->right = rightParent;
    rightParent->left = parent;
 if(x==NULL||y==NULL){
      return RC_OK;
    }
    writeNode(parent, tree);
    writeNode(rightParent, tree);
    writeBtreeHeader(tree);

    // Recursive call to propagate changes to the parent nodes
    return propogateParentNode(tree, parent, rightParent, key);
  }else{
    index =index + 1; // next position is the pointer
    InsertAtArr(parent->childPages, right->pageNum, index);

    int i = parent->value->fill;
    while (i > index) {
        parent->childNode[i] = parent->childNode[i - 1];
        i--;
    }

    parent->childNode[index] = right;

    return writeNode(parent, tree);
  }
}
//mark1 end
void freeNodes(BT_Node *root) {
    if (root == NULL) {
        return;
    }

    BT_Node *child = root;
    do {
        // Finding the first child
        while (!child->isLeaf) {
            child = child->childNode[0];
        }

        BT_Node *parent = child->parent;
        BT_Node *nextNode;

        do {
            // Free each node in the current level
            while (child != NULL) {
                nextNode = child->right;
                destroyBTNode(child);
                child = nextNode;
            }

            // Move to the next level
            if (parent != NULL) {
                child = parent;
                parent = child->parent;
            }
        } while (parent != NULL);
    } while (false);
}

/*
 * Description: Creates a B-tree and initializes the necessary metadata in the file.
 * Param:
 *   idxId - The identifier for the B-tree file.
 *   keyType - The data type of the keys in the B-tree.
 *   n - The order of the B-tree.
 * Return:
 *   OK status if successful, or an appropriate error code.
 */
RC createBtree(char *idxId, DataType keyType, int n) {
   RC reco;
    int back = (PAGE_SIZE - BYTES_BT_HEADER_LEN) / (3 * SIZE_INT);
    if (n > back) {
        return RC_IM_N_TO_LAGE;
    }
    do {
        // Create the page file
        reco = createPageFile(idxId);
        if (RC_OK != reco) {
            break;
        }

        // Open the page file
        SM_FileHandle *fileHandle = (SM_FileHandle *)malloc(sizeof(SM_FileHandle));
        reco = openPageFile(idxId, fileHandle);
        if (RC_OK != reco) {
            free(fileHandle);
            break;
        }

        // Write the B-tree header information to the first page
        int values[] = {n, keyType, 0, 0, 0, 0, 1};
        char *page = (char *)malloc(PAGE_SIZE);
        memcpy(page, values, sizeof(values));
        reco = writeBlock(0, fileHandle, page);
        free(page);

        if (reco != RC_OK) {
            closePageFile(fileHandle);
            free(fileHandle);
            break;
        }

        // Close the page file and return the result
        reco = closePageFile(fileHandle);
        free(fileHandle);
    } while (0);

    return reco;
}

char *printTree(BTreeHandle *tree) {
    BT_Node *rootNode = (*tree).root;
    char *result = newCharArr((*tree).numNodes * (*tree).size * 11 + (*tree).size + (*tree).numNodes + 14);
    if (result == NULL) return result;
    for (int incVal = 0; rootNode != NULL; rootNode = (rootNode->right == NULL && rootNode->isLeaf == 0) ? ((*tree).root, incVal++, (*tree).root) : rootNode->right) {
        printNodeHelper(rootNode, result);
    }
    return result;
}


// Shuts down the index manager
RC shutdownIndexManager() {
    // Perform any cleanup tasks here
    return RC_OK;
}

/*
 * Description: Opens an existing B-tree and initializes the B-tree handle.
 * Param:
 *   tree - A pointer to a BTreeHandle pointer that will store the opened B-tree handle.
 *   idxId - The identifier for the B-tree file.
 * Return:
 *   OK status if successful, or an appropriate error code.
 */
RC openBtree(BTreeHandle **tree, char *idxId) {
    BTreeHandle *trees__ = malloc(sizeof(BTreeHandle));
    if (!trees__) {
        return RC_MALLOC_FAILED;
    }

    BM_BufferPool *bufferM = malloc(sizeof(BM_BufferPool));
    if (!bufferM) {
        freePointer(1, trees__);
        return RC_MALLOC_FAILED;
    }

    RC err = initBufferPool(bufferM, idxId, PER_IDX_BUF_SIZE, RS_LRU, NULL);
    if (err != RC_OK) {
        freePointer(2, bufferM, trees__);
        return err;
    }

    BM_PageHandle *handleOfPage = malloc(sizeof(BM_PageHandle));
    if (!handleOfPage) {
        freePointer(2, bufferM, trees__);
        return RC_MALLOC_FAILED;
    }

    if (pinPage(bufferM, handleOfPage, 0) != RC_OK) {
        freePointer(2, bufferM, trees__);
        freePointer(1, handleOfPage);
        return RC_PIN_ERROR;
    }

    char *poitr = handleOfPage->data;

    trees__->idxId = idxId;
    trees__->mgmtData = bufferM;

    for (int i = 0; i < 7; ++i) {
        switch (i) {
            case 0:
                memcpy(&trees__->size, poitr, SIZE_INT);
                break;
            case 1:
                memcpy(&trees__->keyType, poitr, SIZE_INT);
                break;
            case 2:
                memcpy(&trees__->whereIsRoot, poitr, SIZE_INT);
                break;
            case 3:
                memcpy(&trees__->numNodes, poitr, SIZE_INT);
                break;
            case 4:
                memcpy(&trees__->numEntries, poitr, SIZE_INT);
                break;
            case 5:
                memcpy(&trees__->depth, poitr, SIZE_INT);
                break;
            case 6:
                memcpy(&trees__->nextPage, poitr, SIZE_INT);
                break;
        }
        poitr += SIZE_INT;
    }

    err = unpinPage(bufferM, handleOfPage);
    freePointer(1, handleOfPage);

    if (err != RC_OK) {
        closeBtree(trees__);
        return err;
    }

    err = loadBtree(trees__);
    if (err != RC_OK) {
        closeBtree(trees__);
        return err;
    }

    *tree = trees__;
    return RC_OK;
}

/*
 * Description: Closes a B-tree, releasing associated resources.
 * Param:
 *   tree - The B-tree handle.
 * Return:
 *   OK status if successful, or an appropriate error code.
 */
RC closeBtree(BTreeHandle *tree) {
    // Free allocated memory for B-tree nodes
    freeNodes(tree->root);

    // Shutdown the buffer pool associated with the B-tree
    shutdownBufferPool(tree->mgmtData);

    // Free allocated memory for the B-tree handle and buffer pool
    free(tree->mgmtData);
    free(tree);
    tree = NULL;

    // Return OK if the B-tree is successfully closed
    return RC_OK;
}

/*
  - description: Deletes a B-tree file
  - param:
    1. idxId - The identifier for the B-tree file
  - return: RC_FS_ERROR if there is an error in unlinking,
            or RC_OK if the file is successfully unlinked
*/
RC deleteBtree(char *idxId) {
    // Attempt to unlink the file
    if (unlink(idxId) == -1) {
        // Error in unlinking
        return RC_FS_ERROR;
    }

    // File successfully unlinked
    return RC_OK;
}

/*
  - description: Retrieves the number of nodes in the B-tree
  - param:
    1. tree - The B-tree handle
    2. result - A pointer to an integer that will store the result (number of nodes)
  - return: OK status if successful, or an appropriate error code
*/
RC getNumNodes(BTreeHandle *tree, int *result) {
    // Check if the B-tree handle and result pointer are valid
    if (tree == NULL || result == NULL) {
        return RC_FILE_NOT_FOUND;
    }

    // Assign the value of numNodes from the B-tree handle to the result
    *result = tree->numNodes;

    // Return OK status to indicate successful execution
    return RC_OK;
}

/*
  - description: Retrieves the number of entries (key-value pairs) in the B-tree
  - param:
    1. tree - The B-tree handle
    2. result - A pointer to an integer that will store the result (number of entries)
  - return: OK status if successful, or an appropriate error code
*/
RC getNumEntries(BTreeHandle *tree, int *result) {
    if (tree == NULL || result == NULL) {
        return RC_INVALID_HANDLE;
    }

    // Approach 1: Direct Assignment
    *result = tree->numEntries;

    return RC_OK;
}

/*
  - description: Retrieves the data type of the keys in the B-tree
  - param:
    1. tree - The B-tree handle
    2. result - A pointer to a DataType that will store the result (data type of keys)
  - return: OK status if successful, or an appropriate error code
*/
RC getKeyType(BTreeHandle *tree, DataType *result) {
    if (tree == NULL || result == NULL) {
        return RC_INVALID_HANDLE;
    }

    DataType tempKeyType = tree->keyType;
    *result = tempKeyType;

    return RC_OK;
}



/*
    - description : Finds the key in the B-tree and returns the corresponding RID (Record ID).
    - param :
        1. tree - The B-tree handle
        2. key - The key to be found
    3. result -  The pointer to the result RID
    - return : RC_OK if the key is found and the RID is set successfully,
             RC_IM_KEY_NOT_FOUND if the key is not found
*/
RC findKey(BTreeHandle *tree, Value *key, RID *result) {
    if (tree == NULL || key == NULL || result == NULL) {
        return RC_INVALID_HANDLE;
    }

    BT_Node *targetNode = findNodeByKey(tree, key->v.intV);
    if (targetNode == NULL) {
        return RC_IM_KEY_NOT_FOUND;
    }

    int target = 0;
    int i = dynamicArrSearch(targetNode->value, key->v.intV, &target);

    if (i >= 0) {
        *(int *)&result->slot = targetNode->leafRIDSlots->elements[i];
        *(int *)&result->page = targetNode->leafRIDPages->elements[i];

        return RC_OK; // SUCCESS
    }

    return RC_IM_KEY_NOT_FOUND; // RC_IM_KEY_NOT_FOUND because it is an error
}

/*
  Description: Closes a B-tree scan, releasing allocated memory.
  Parameters:
  handle: A pointer to the scan handle.
  Returns:
  RC_OK if the scan is successfully closed.
*/
RC closeTreeScan(BT_ScanHandle *handle) {
    free(handle->mgmtData);
    free(handle);
    return RC_OK;
}


/*
    - description : Inserts a key-value pair into a B-tree
    - param :
        1. tree - The B-tree handle
        2. key - The key to be found
    3. rid - The record ID associated with the key
    - return : RC_OK if the insertion is successful, RC_IM_KEY_ALREADY_EXISTS if the key already exists in the B-tree, or an appropriate error code
*/
RC insertKey(BTreeHandle *tree, Value *key, RID rid)
{
  int x=5;
  BT_Node *targetNode = findNodeByKey(tree, key->v.intV);
  int target = 0;
  if (targetNode == NULL)
  {
    targetNode = createBTNode(tree->size, 1, tree->nextPage);
        tree->depth = tree->depth + 1;
int y=x+1;
    tree->whereIsRoot = targetNode->pageNum;
    tree->root = targetNode;
        tree->numNodes = tree->numNodes + 1;
        y++;
    tree->nextPage = tree->nextPage + 1;
    x++;
    writeBtreeHeader(tree); 
  }
  int z=x-1;
  int rightindex=5;
  
  int index = dynamicArrSearch(targetNode->value, key->v.intV, &target);
  rightindex=rightindex+x+z;
  if (index >= 0)
  {  
    if (x <= 0)
  {
    return RC_IM_KEY_ALREADY_EXISTS;
  }                                
    return RC_IM_KEY_ALREADY_EXISTS; 
  }
  index = InsertArr(targetNode->value, key->v.intV);
   BT_Node *dividable;
  if (index < 0)
  {                                        
    x++;                                                               
    dividable = createBTNode(tree->size + 1, 1, -1); 
    memcpy(dividable->value->elements, targetNode->value->elements, SIZE_INT * targetNode->value->fill);
    x++;
    dividable->value->fill = targetNode->value->fill;
    int leftFill;
    memcpy(dividable->leafRIDPages->elements, targetNode->leafRIDPages->elements, SIZE_INT * targetNode->leafRIDPages->fill);
    dividable->leafRIDPages->fill = targetNode->leafRIDPages->fill;
     BT_Node *rightChild;
    memcpy(dividable->leafRIDSlots->elements, targetNode->leafRIDSlots->elements, SIZE_INT * targetNode->leafRIDSlots->fill);
    dividable->leafRIDSlots->fill = targetNode->leafRIDSlots->fill;
    int rightFill;
    index = InsertArr(dividable->value, key->v.intV);
    InsertAtArr(dividable->leafRIDPages, rid.page, index);
    InsertAtArr(dividable->leafRIDSlots, rid.slot, index);

    leftFill = ceil((float)dividable->value->fill / 2);
     rightFill = dividable->value->fill - leftFill;
    rightChild = createBTNode(tree->size, 1, tree->nextPage); 
    int leftIndex=4;
    tree->numNodes++;
    tree->nextPage++;

    targetNode->leafRIDSlots->fill = leftFill;
    targetNode->leafRIDPages->fill = targetNode->leafRIDSlots->fill;
    targetNode->value->fill = targetNode->leafRIDPages->fill;

    memcpy(targetNode->value->elements, dividable->value->elements, SIZE_INT * leftFill);
    leftIndex++;
    memcpy(targetNode->leafRIDPages->elements, dividable->leafRIDPages->elements, SIZE_INT * leftFill);
    rightindex++;
    memcpy(targetNode->leafRIDSlots->elements, dividable->leafRIDSlots->elements, SIZE_INT * leftFill);

    rightChild->leafRIDSlots->fill = rightFill;
    leftIndex = rightindex+1;
    rightChild->leafRIDPages->fill = rightChild->leafRIDSlots->fill;
    leftIndex++;
    rightChild->value->fill = rightChild->leafRIDPages->fill;

    memcpy(rightChild->value->elements, dividable->value->elements + leftFill, SIZE_INT * rightFill);
    leftIndex++;
    memcpy(rightChild->leafRIDPages->elements, dividable->leafRIDPages->elements + leftFill, SIZE_INT * rightFill);
    memcpy(rightChild->leafRIDSlots->elements, dividable->leafRIDSlots->elements + leftFill, SIZE_INT * rightFill);

    destroyBTNode(dividable); 
    rightindex++;

    rightChild->right = targetNode->right;
    if (rightChild->right != NULL)
    {
      leftIndex++;
      rightChild->right->left = rightChild;
    }
    rightChild->left = targetNode;
    rightindex++;
    targetNode->right = rightChild;
    writeNode(rightChild, tree);
    leftIndex++;
    writeNode(targetNode, tree);
    propogateParentNode(tree, targetNode, rightChild, rightChild->value->elements[0]);
    rightindex++;
    tree->numEntries = tree->numEntries + 1; 
    writeBtreeHeader(tree);                 
    return RC_OK;                            
  }else{
        tree->numEntries = tree->numEntries + 1;          

    InsertAtArr(targetNode->leafRIDPages, rid.page, index); 
    InsertAtArr(targetNode->leafRIDSlots, rid.slot, index); 
    writeBtreeHeader(tree);     
       rightindex++;                    
    return RC_OK;   
  }
  int leftIndex1=2;
  leftIndex1++;
  return RC_OK; 
}

/*
  Description: Deletes a key from the B-tree, if present, and updates associated leaf page information.
  Parameters:
  tree: Pointer to the B-tree handle.
  key: Pointer to the value to be deleted.
  Returns:
  RC_OK if the key is successfully deleted.
  Notes:
  The function locates the node containing the key to be deleted.
  If the key is not found, the function returns without making any changes.
  If the key is found, it is removed from the node's value array, along with associated leaf page and slot information.
  The B-tree header and the node containing the modified data are updated.
*/
RC deleteKey(BTreeHandle *tree, Value *key) {
    BT_Node *targetNode = findNodeByKey(tree, key->v.intV);
    int target = 0;
    if (!targetNode) {
        return RC_OK;
    }
    
    int i = dynamicArrSearch(targetNode->value, key->v.intV, &target);
    i = (i >= 0) ? i : 0; // Ternary operator to handle negative index
    
    if (i < 0) {
        return RC_OK;
    }

    int *pagesPtr = &(targetNode->leafRIDPages->elements[i]);
    int *valuesPtr = &(targetNode->value->elements[i]);
    int *slotsPtr = &(targetNode->leafRIDSlots->elements[i]);
    
    for (int j = i; j < targetNode->value->fill - 1; j++) {
        *pagesPtr = *(pagesPtr + 1);
        *valuesPtr = *(valuesPtr + 1);
        *slotsPtr = *(slotsPtr + 1);
        pagesPtr++;
        valuesPtr++;
        slotsPtr++;
    }

    targetNode->value->fill--;
    tree->numEntries--;
    writeBtreeHeader(tree);
    writeNode(targetNode, tree);
    return RC_OK;
}



/*
  Description: Closes a B-tree scan, releasing allocated memory.
  Parameters:
  handle: A pointer to the scan handle.
  Returns:
  RC_OK if the scan is successfully closed.
*/
RC openTreeScan(BTreeHandle *tree, BT_ScanHandle **handle) {
    ScanMgmtInfo *scanMgmtInfo = (ScanMgmtInfo *)malloc(sizeof(ScanMgmtInfo));
    BT_ScanHandle *btScanHandle = (BT_ScanHandle *)malloc(sizeof(BT_ScanHandle));
    scanMgmtInfo->currentNode = tree->root;
    for (int i = 0; !scanMgmtInfo->currentNode->isLeaf; i++) {
        scanMgmtInfo->currentNode = scanMgmtInfo->currentNode->childNode[0];
    }
    btScanHandle->tree = tree;
    btScanHandle->mgmtData = scanMgmtInfo;
    scanMgmtInfo->elementIndex = 0;
    *handle = btScanHandle;
    return RC_OK;
}

/*
  Description: Advances the B-tree scan to the next entry and retrieves the Record ID (RID).
  Parameters:
  handle - The scan handle.
  result - A pointer to a RID where the result will be stored.
  Returns:
  OK status if successful.
  RC_IM_NO_MORE_ENTRIES if there are no more entries.
  An appropriate error code in case of failure.
*/
RC nextEntry (BT_ScanHandle *handle, RID *result){
  ScanMgmtInfo *scanMgmtInfo;
  scanMgmtInfo = handle->mgmtData;
  if((*scanMgmtInfo).elementIndex >= (*scanMgmtInfo).currentNode->leafRIDPages->fill) { // finding the left most leaf
    if((*scanMgmtInfo).elementIndex != (*scanMgmtInfo).currentNode->value->fill){
      (*scanMgmtInfo).currentNode = (*scanMgmtInfo).currentNode->right;
      (*scanMgmtInfo).elementIndex = 0;
    } else {
      if((*scanMgmtInfo).currentNode->right!=NULL) {
        (*scanMgmtInfo).currentNode = (*scanMgmtInfo).currentNode->right;
        (*scanMgmtInfo).elementIndex = 0;
      }
      else {
        return RC_IM_NO_MORE_ENTRIES;
      }
      
    }
  }
  (*result).slot = (*scanMgmtInfo).currentNode->leafRIDSlots->elements[(*scanMgmtInfo).elementIndex];
  (*result).page = (*scanMgmtInfo).currentNode->leafRIDPages->elements[(*scanMgmtInfo).elementIndex];

  (*scanMgmtInfo).elementIndex= (*scanMgmtInfo).elementIndex+1;
  return RC_OK;
}


