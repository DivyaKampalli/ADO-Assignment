#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "dberror.h"
#include "dt.h"
#include "storage_mgr.h"

typedef int PageNumber;
// Replacement Strategies
typedef enum ReplacementStrategy {
    RS_FIFO = 0,
    RS_LRU = 1,
    RS_CLOCK = 2,
    RS_LFU = 3,
    RS_LRU_K = 4
} ReplacementStrategy;

// Data Types and Structures
typedef int PageNumber;
#define NO_PAGE -1


// This structure represents one page frame in the buffer pool (memory).
typedef struct Page
{
	SM_PageHandle data; // Actual data of the page
	PageNumber pageNum; // An identification integer given to each page
	int dirtyBit; // Used to indicate whether the contents of the page has been modified by the client
	int fixCount; // Used to indicate the number of clients using that page at a given instance
	int hitNum;   // Used by LRU algorithm to get the least recently used page	
	int refNum;   // Used by LFU algorithm to get the least frequently used page
} PageFrame;



typedef struct BM_BufferPool {
    char *pageFile;
    int numPages;
    ReplacementStrategy strategy;
    PageFrame *mgmtData; // use this one to store the bookkeeping info your buffer
    // LRU_K_History *history;
} BM_BufferPool;

typedef struct BM_PageHandle {
    PageNumber pageNum;
    char *data;
} BM_PageHandle;


// convenience macros
#define MAKE_POOL() ((BM_BufferPool *)malloc(sizeof(BM_BufferPool)))

#define MAKE_PAGE_HANDLE() ((BM_PageHandle *)malloc(sizeof(BM_PageHandle)))

// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numPages, ReplacementStrategy strategy,
                  void *stratData);
RC shutdownBufferPool(BM_BufferPool *const bm);
RC forceFlushPool(BM_BufferPool *const bm);

void persistPage(BM_BufferPool *const bm, PageFrame *pageFrame);
void updatePageFrame(PageFrame *destination, const PageFrame *source);

// Buffer Manager Interface Access Pages
RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page);
RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page);
RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page);
RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page,
           const PageNumber pageNum);

// Statistics Interface
PageNumber *getFrameContents(BM_BufferPool *const bm);
bool *getDirtyFlags(BM_BufferPool *const bm);
int *getFixCounts(BM_BufferPool *const bm);
int getNumReadIO(BM_BufferPool *const bm);
int getNumWriteIO(BM_BufferPool *const bm);

// LRU_K functions
// void updateLRU_K_History(LRU_K_History *history, PageNumber pageNum);
// void freeLRU_K_History(LRU_K_History *history);
// PageNumber getLRU_K_HistorySize(LRU_K_History *history);

#endif
#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "dberror.h"
#include "dt.h"
#include "storage_mgr.h"

typedef int PageNumber;
// Replacement Strategies
typedef enum ReplacementStrategy {
    RS_FIFO = 0,
    RS_LRU = 1,
    RS_CLOCK = 2,
    RS_LFU = 3,
    RS_LRU_K = 4
} ReplacementStrategy;

// Data Types and Structures
typedef int PageNumber;
#define NO_PAGE -1


// This structure represents one page frame in the buffer pool (memory).
typedef struct Page
{
	SM_PageHandle data; // Actual data of the page
	PageNumber pageNum; // An identification integer given to each page
	int dirtyBit; // Used to indicate whether the contents of the page has been modified by the client
	int fixCount; // Used to indicate the number of clients using that page at a given instance
	int hitNum;   // Used by LRU algorithm to get the least recently used page	
	int refNum;   // Used by LFU algorithm to get the least frequently used page
} PageFrame;



typedef struct BM_BufferPool {
    char *pageFile;
    int numPages;
    ReplacementStrategy strategy;
    PageFrame *mgmtData; // use this one to store the bookkeeping info your buffer
    // LRU_K_History *history;
} BM_BufferPool;

typedef struct BM_PageHandle {
    PageNumber pageNum;
    char *data;
} BM_PageHandle;


// convenience macros
#define MAKE_POOL() ((BM_BufferPool *)malloc(sizeof(BM_BufferPool)))

#define MAKE_PAGE_HANDLE() ((BM_PageHandle *)malloc(sizeof(BM_PageHandle)))

// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numPages, ReplacementStrategy strategy,
                  void *stratData);
RC shutdownBufferPool(BM_BufferPool *const bm);
RC forceFlushPool(BM_BufferPool *const bm);

void persistPage(BM_BufferPool *const bm, PageFrame *pageFrame);
void updatePageFrame(PageFrame *destination, const PageFrame *source);

// Buffer Manager Interface Access Pages
RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page);
RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page);
RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page);
RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page,
           const PageNumber pageNum);

// Statistics Interface
PageNumber *getFrameContents(BM_BufferPool *const bm);
bool *getDirtyFlags(BM_BufferPool *const bm);
int *getFixCounts(BM_BufferPool *const bm);
int getNumReadIO(BM_BufferPool *const bm);
int getNumWriteIO(BM_BufferPool *const bm);

// LRU_K functions
// void updateLRU_K_History(LRU_K_History *history, PageNumber pageNum);
// void freeLRU_K_History(LRU_K_History *history);
// PageNumber getLRU_K_HistorySize(LRU_K_History *history);

#endif
