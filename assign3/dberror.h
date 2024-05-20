#ifndef DBERROR_H
#define DBERROR_H

#include "stdio.h"

/* module wide constants */
#define PAGE_SIZE 4096

/* return code definitions */
typedef int RC;

#define RC_OK 0
#define RC_FILE_NOT_FOUND 1
#define RC_FILE_HANDLE_NOT_INIT 2
#define RC_WRITE_FAILED 3
#define RC_READ_NON_EXISTING_PAGE 4
#define RC_PAGE_HANDLE_NOT_INIT 12
#define RC_INVALID_NUMBER_OF_PAGES 14
#define RC_ERROR 400 // Added a new definiton for ERROR
#define RC_PINNED_PAGES_IN_BUFFER 500 // Added a new definition for Buffer Manager

#define RC_FILE_ALREADY_EXISTS 58 //
#define RC_READING_FAILED 5 //
#define RC_ERROR_CLOSING 6 //
#define RC_ERROR_DELETING 7 //
#define RC_BUFFER_POOL_NOT_INIT 15
#define RC_BUFFER_POOL_SHUTDOWN_ERROR 16
#define RC_PAGE_NOT_PINNED 17
#define RC_PAGE_NOT_IN_FRAMELIST 18
#define RC_POOL_NOT_OPEN 19
#define RC_NEGATIVE_PAGE_NUM 20
#define RC_IMPOSSIBLE_VALUE 21
#define RC_MEM_ALLOC_FAILED 22
#define RC_RM_NO_MORE_SLOTS 23
#define RC_UNPIN_PAGE_FAILED 24
#define RC_PIN_PAGE_FAILED 25

#define RC_RM_COMPARE_VALUE_OF_DIFFERENT_DATATYPE 200
#define RC_RM_EXPR_RESULT_IS_NOT_BOOLEAN 201
#define RC_RM_BOOLEAN_EXPR_ARG_IS_NOT_BOOLEAN 202
#define RC_RM_NO_MORE_TUPLES 203
#define RC_RM_NO_PRINT_FOR_DATATYPE 204
#define RC_RM_UNKOWN_DATATYPE 205

#define RC_IM_KEY_NOT_FOUND 300
#define RC_IM_KEY_ALREADY_EXISTS 301
#define RC_IM_N_TO_LAGE 302
#define RC_IM_NO_MORE_ENTRIES 303

// Added new definitions for Record Manager
#define RC_RM_NO_TUPLE_WITH_GIVEN_RID 600
#define RC_SCAN_CONDITION_NOT_FOUND 601

/* holder for error messages */
extern char *RC_message;

/* print a message to standard out describing the error */
extern void printError (RC error);
extern char *errorMessage (RC error);

#define THROW(rc,message) \
		do {			  \
			RC_message=message;	  \
			return rc;		  \
		} while (0)		  \

// check the return code and exit if it is an error
#define CHECK(code)							\
		do {									\
			int rc_internal = (code);						\
			if (rc_internal != RC_OK)						\
			{									\
				char *message = errorMessage(rc_internal);			\
				printf("[%s-L%i-%s] ERROR: Operation returned error: %s\n",__FILE__, __LINE__, __TIME__, message); \
				free(message);							\
				exit(1);							\
			}									\
		} while(0);


#endif
