# Assignment - 2

## Buffer Manager

### Team Members:
- Abhiram Ravipati                 - aravipati@hawk.iit.edu      - A20539084 
- Divya Kampalli                   - kdivya1@hawk.iit.edu        - A20539479   
- Sai Jayanth Rajamahendravaram    - srajamahendram@hawk.iit.edu - A20539434
- Sri Samhitha Bobba               - sbobba1@hawk.iit.edu        - A20541559

### Contributions
- Abhiram Ravipati                - LRU(), initBufferPool(), pinPage(), getDirtyFlags()
- Divya Kampalli                  - FIFO(), LFU(), LRU_K(), shutdownBufferPool(), unpinPage(), forcePage(), getFrameContents(), getFixCounts()
- Sai Jayanth Rajamahendravaram   - CLOCK(), forceFlushPool(), markDirty()
- Sri Samhitha Bobba              - getNumReadIO(), getNumWriteIO()

#### HOW TO EXECUTE THE SCRIPT

1. Open terminal 

2. Run the command: "make"

3. Run the command "./test_assign2_1"

4. Run the command "./test_assign2_2"

4. To remove object files run the command "make clean"


## Overview of the Assignment:
The goal of this assignment is to implement a simple buffer manager that manages a fixed number of pages in memory, referred to as page frames. These page frames represent pages from a page file managed by the storage manager (implemented in assignment 1). The combination of a page file and the page frames storing pages from that file is known as a Buffer Pool.


### Buffer Manager Functions:
1. FIFO():

	- We will implement the First-In-First-Out page replacement strategy.
	- We will find the first page frame with fixCount 0 and replace it with the new page.

2. LFU():

	- We will implement the Least Frequently Used (LFU) page replacement strategy.
	- We will identify the page frame with the least number of references and replace it with the new page.

3. LRU():

	- We will implement the Least Recently Used (LRU) page replacement strategy.
	- We will identify the page frame that has not been recently used and replace it with the new page.

4. LRU_K():

	- We will implement the LRU_K page replacement strategy.
	- Similar to LRU but considering a window of K recent accesses to determine the least recently used page.

5. CLOCK():

	- We will implement the CLOCK page replacement strategy.
	- We will use a clock-like algorithm to find the first page frame with hitNum 0 and replace it with the new page.

6. initBufferPool():

	- We will initialize the buffer pool with the specified number of page frames and other required information.
	- We will allocate memory for page frames, set initial values, and prepare the buffer pool for managing pages.

7. shutdownBufferPool():

	- We will destroy the buffer pool by freeing allocated memory.
	- We will check for pinned pages and prevent shutdown if there are still pages in use.

8. forceFlushPool():

	- We will write dirty pages in the buffer pool back to the page file on disk.
	- We will ensure that modifications are persisted before shutting down or replacing pages.

9. markDirty():

	- We will mark a page in the buffer pool as dirty, indicating that it has been modified.
	- It will be used to track changes that need to be flushed to disk.

10. unpinPage():

	- We will decrease the fixCount for a page in the buffer pool, indicating that it is no longer in use.
	- We will check for errors, such as attempting to unpin an already unpinned page.

11. forcePage():

	- We will write the content of a specific page in the buffer pool back to the page file on disk.
	- We will mark the page as not dirty after writing.

12. pinPage():

	- We will add a page to the buffer pool by pinning it.
	- We will handle cases where the buffer pool is initially empty or full, using different page replacement strategies.

13. getFrameContents():

	- We will return an array of page numbers currently stored in the buffer pool.	
	- It will be useful for understanding the current state of the buffer pool.

14. getDirtyFlags():

	- We will return an array of booleans representing the dirtyBit status of each page in the buffer pool.
	- It will indicate whether a page has been modified.

15. getFixCounts():

	- We will return an array of integers representing the fixCount of each page in the buffer pool.
	- It will indicate how many clients currently have the page pinned.

16. getNumReadIO():

	- We will return the number of pages read from disk since the buffer pool was initialized.
	- It will help in monitoring disk read operations.

17. getNumWriteIO():

	- We will return the number of pages written to the page file on disk since the buffer pool was initialized.
	- It will help in monitoring disk write operations.