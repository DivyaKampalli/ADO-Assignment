#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<math.h>
#include<errno.h>
#include "const.h"

#include "storage_mgr.h"

FILE *file;

extern void initStorageManager (void) {
	// Initialising file pointer i.e. storage manager.
	file = NULL;
}
int checkFileHandlerIsInit(SM_FileHandle *file_handler)
{
    return (file_handler->mgmtInfo != NULL);
}


// Function to create a new page file with the initial file size as one page. The page should be filled with '\0' bytes
RC createPageFile(char *fileName)
{
    // Try to open the file in write and read mode, creating it if it doesn't exist
    file = fopen(fileName, "wb");

    // Check if the file opening was unsuccessful
    if (!file)
    {
        // Return an error code indicating the file was not found
        return RC_FILE_NOT_FOUND;
    }
    else
    {
        // Allocate memory for a page-sized buffer and initialize it with null characters
        SM_PageHandle buffer_file = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char));
        memset(buffer_file, '\0', PAGE_SIZE);

        // Write the null-filled buffer content to the file
        size_t no_characters = fwrite(buffer_file, sizeof(char), PAGE_SIZE, file);

        // Check if the number of characters written is less than the page size
        if (no_characters < PAGE_SIZE)
        {
            // Return an error code indicating the write operation failed
            return RC_WRITE_FAILED;
        }

        // Free the allocated memory before returning
        free(buffer_file);

        // Close the file
        fclose(file);
    }
    
    // Return success code
    return RC_OK;
}


// Function to create an open page file for opening an existing page file which should return RC_FILE_NOT_FOUND if the file dont exist.
RC openPageFile(char *file_name, SM_FileHandle *file_handler)
{
    // Attempting to open the file in read only mode
    file = fopen(file_name, "r");

    // Checking if the file opening was not successful
    if (!file)
    {
        // Returning an error code if the file is not found
        return RC_FILE_NOT_FOUND;
    }
    else
    {
        // Retrieving the file information, such as size, using fstat
        struct stat file_information;
        // Intializing currrent page position to the beginning of the file
        file_handler->curPagePos = 0;
        // Set the file name in the file handler
        file_handler->fileName = file_name;
        if (fstat(fileno(file), &file_information) < 0)
        {
            // Closing the file before returning the error code
            fclose(file);
            // Returning an error code if fstat operation fails
            return RC_ERROR;
        }

        int size_of_page = PAGE_SIZE;
        int total_size = file_information.st_size;

        // Calcualting the total number of pages based on the file size and page size
        file_handler->totalNumPages = total_size / size_of_page;
        // Closing the file after obtaining necessary information
        fclose(file);
        // Returning success code if the file is successfully opened and information is retrieved
        return RC_OK;
    }
    fclose(file);
    return RC_OK;
}

// Function to close an open page file
RC closePageFile(SM_FileHandle *file_handler)
{
    // Checking if the file is not already closed
    if (file != NULL)
    {
        // Close the file
        fclose(file);
    }
    // Returning success code indicating successful file closure
    return RC_OK;
}


// Function to destroy page file
RC destroyPageFile(char *fileName)
{
    // Indicator for file present
    int isPresent = 0;
    // Attempting to delete the file using the remove function
    if (remove(fileName) == isPresent)
    {
        // Returning success code if file is removed successfully
        return RC_OK;
    }
    else
    {
        // If remove was unsuccessful, check if the file exists using access function
        if (access(fileName, F_OK) != isPresent)
        {
            // Returning an error code if the file is not found
            return RC_FILE_NOT_FOUND;
        }
    }
    return RC_ERROR;
}


// Function to create read block
RC readBlock(int pageNum, SM_FileHandle *file_handler, SM_PageHandle pageData)
{
    // Attempting to open the file in read-only mode
    file = fopen(file_handler->fileName, "r");
    // Checking if the file opening was not successful
    if (!file)
    {
        // Returning an error code if the file is not found or cannot be opened
        return RC_FILE_NOT_FOUND;
    }
    // Checking if the page number is greater than total number of pages
    if (pageNum >= ((file_handler->totalNumPages)))
    {
        return RC_READING_FAILED;
    }
    // Checking if the file handler is properly initialized
    if (!file_handler)
    {
        fclose(file);
        // Returning an error code if the file handler is not properly initialized
        return RC_FILE_HANDLE_NOT_INIT;
    }
    // Checking if page number is less than zero
    if (pageNum < 0)
    {

        return RC_READING_FAILED;
    }
    // Checking if page number id less than 0 and page number is greater than equal to total number of pages
    if (pageNum < 0 || pageNum >= file_handler->totalNumPages)
    {
        fclose(file);
        return RC_READ_NON_EXISTING_PAGE;
    }
    // Calculating the page info by using page numebr and page size
    int page_info = pageNum * PAGE_SIZE;
    // Setting the file position indicator to the calculated position
    int seekResult = fseek(file, page_info, SEEK_SET);
    // Checking for seek operation failure
    if (seekResult != 0)
    {
        // Handling specific error cases
        if (errno == EFBIG)
        {
            return RC_READ_NON_EXISTING_PAGE;
        }
        if (errno == EBADF)
        {
            return RC_FILE_HANDLE_NOT_INIT;
        }
        return RC_ERROR;
    }
    file_handler->curPagePos = page_info;
    // Attempting to read the page data into the provided buffer
    int readResult = fread(pageData, sizeof(char), PAGE_SIZE, file);
    // Checking if the read operation was not successful
    if (readResult != PAGE_SIZE)
    {
        fclose(file);
        // Returning an error code for a failed read operation
        return RC_READING_FAILED;
    }
    // Updating the current page position in the file handler
    file_handler->curPagePos = (pageNum + 1) * PAGE_SIZE;
    // Closing the file after reading
    fclose(file);

    return RC_OK;
}

// Function to get block position
int getBlockPos(SM_FileHandle *file_handler)
{
    // Checking if the file handler is properly initialized
    if (!file_handler)
    {
        // Returning an error code if the file handler not properly initialized
        return RC_FILE_HANDLE_NOT_INIT;
    }
    // Returning the current page position from the file handler
    return file_handler->curPagePos;
}

// Function to read the first block
RC readFirstBlock(SM_FileHandle *file_handler, SM_PageHandle pageData)
{
    // Checking if the file handler is properly intialized
    if (!file_handler)
    {
        // Returning an error code if the file handler is not properly intialized
        return RC_FILE_HANDLE_NOT_INIT;
    }
    // Checking if the page data buffer is properly intialized
    if (!pageData)
    {
        return RC_PAGE_HANDLE_NOT_INIT;
    }
    // Setting the page number to read as the first block
    int initialBlock = 0;
    // Call the readblock fucntion to read the first block using the specified page number
    return readBlock(initialBlock, file_handler, pageData);
}


RC readPreviousBlock(SM_FileHandle *file_handler, SM_PageHandle pageData)
{
    if (!file_handler)
    {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    if (!pageData)
    {
        return RC_PAGE_HANDLE_NOT_INIT;
    }
    int current_page_number = file_handler->curPagePos / PAGE_SIZE;
    int previous_page = current_page_number - 1;
    if (previous_page < 0 || previous_page >= file_handler->totalNumPages)
    {
        return RC_READ_NON_EXISTING_PAGE;
    }
    return readBlock(previous_page, file_handler, pageData);
}

// Function to read the curent block
RC readCurrentBlock(SM_FileHandle *file_handler, SM_PageHandle pageData)
{
    // Checking if the file handler is properly initialized
    if (!file_handler)
    {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    // Checking if the pageData buffer is properly initialized
    if (!pageData)
    {
        return RC_PAGE_HANDLE_NOT_INIT;
    }
    // Calculating the current page number
    int current_page_number = file_handler->curPagePos / PAGE_SIZE;
    if (current_page_number < 0 || current_page_number >= file_handler->totalNumPages)
    {
        return RC_READ_NON_EXISTING_PAGE;
    }
    // Calling the readBlock function
    return readBlock(current_page_number, file_handler, pageData);
}

// Function to read next block
RC readNextBlock(SM_FileHandle *file_handler, SM_PageHandle pageData)
{
    // Checking if the file handler is properly initialized
    if (!file_handler)
    {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    // Checking if the pageData buffer is properly initialized
    if (!pageData)
    {
        return RC_PAGE_HANDLE_NOT_INIT;
    }
    // Calculating the current page number
    int current_page_number = file_handler->curPagePos / PAGE_SIZE;
    // Calculating the page number of the next block
    int next_page = current_page_number + 1;
    // Calling the readBlock function to read the next block using the specified page number
    return readBlock(next_page, file_handler, pageData);
}

// Function to read last block
RC readLastBlock(SM_FileHandle *file_handler, SM_PageHandle pageData)
{
    // Checking if the file handler is properly initialized
    if (!file_handler)
    {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    // Checking if the pageData buffer is properly initialized
    if (!pageData)
    {
        return RC_PAGE_HANDLE_NOT_INIT;
    }
    // Calculating the page number of the last block
    int lastBlock = file_handler->totalNumPages - 1;
    // Checking if the calculated last page number is out of bounds
    if (lastBlock >= file_handler->totalNumPages)
    {
        return RC_READ_NON_EXISTING_PAGE;
    }
    return readBlock(lastBlock, file_handler, pageData);
}

// Function to write block
RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {

    // Throw error if condition is not satisfied
    if(pageNum > fHandle->totalNumPages){
        return RC_WRITE_FAILED;
    }
    file = fopen(fHandle->fileName, "r+");

    if(pageNum < 0){
        return RC_WRITE_FAILED;
    }
    if (memPage == NULL) {
        return RC_WRITE_FAILED;
    }

    
    // Throw error if pageFile is NULL
    if(file != NULL){ 
        
        int startPosition = PAGE_SIZE * pageNum;

            if(pageNum == 0) { 
                int i;
                //Writing data to non-first page
                fseek(file, startPosition, SEEK_SET);   
                long currentPosition; 
                for (i = 0; i < PAGE_SIZE; i++)
                {
                    // Store the current position in the file
                    currentPosition = ftell(file);

                    // Checking if it is the end of the file. If yes, then append an empty block.
                    if (currentPosition >= startPosition + PAGE_SIZE)
                    {
                        appendEmptyBlock(fHandle);
                    }

                    // Writing a character from memPage to page file
                   fwrite(&memPage[i], sizeof(char), 1, file);
                }

                // Setting the current page position to the cursor(pointer) position of the file stream
                fHandle->curPagePos = ftell(file); 

                // Closing file stream so that all the buffers are flushed.
                fclose(file);   
            } else {    
                // Writing data to the first page.
                fHandle->curPagePos = startPosition;
                fclose(file);
                writeCurrentBlock(fHandle, memPage);
            }
            return RC_OK;
        }else{
            return RC_WRITE_FAILED;
        }
}
extern RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (file == NULL)
        return RC_FILE_NOT_FOUND;
    // Opening file stream in read & write mode. 'r+' mode opens the file for both reading and writing.
    file = fopen(fHandle->fileName, "r+");

    // Checking if file was successfully opened.
    

    // Appending an empty block to make some space for the new content.
    appendEmptyBlock(fHandle);

    // Initializing file pointer to the beginning of the current page.
    fseek(file, fHandle->curPagePos, SEEK_SET);

    // Writing memPage contents to the file.
    fwrite(memPage, sizeof(char), PAGE_SIZE, file);

    // Setting the current page position to the cursor(pointer) position of the file stream
    fHandle->curPagePos = ftell(file);

    // Closing file stream so that all the buffers are flushed.
    fclose(file);
    return RC_OK;
}




// Function to append the empty block
RC appendEmptyBlock(SM_FileHandle *file_handler)
{
    // Size of a character in bytes
    int csize = sizeof(char);
    // Checking if the file handler is properly initialized
    if (!file_handler)
    {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    // Allocating memory for an empty page
    SM_PageHandle empty_page = (SM_PageHandle)calloc(PAGE_SIZE, csize);
    // Moving the file position indicator to the end of the file
    int res = fseek(file, 0, SEEK_END);
    // Checking if the seek operation was unsuccessful
    if (res != 0)
    {
        // Free the allocated memory before returning the error code
        free(empty_page);
        return RC_WRITE_FAILED;
    }
    else
    {
        // Writing the empty page to the file to append an empty block
        fwrite(empty_page, csize, PAGE_SIZE, file);
    }
    // Freeing the allocated memory after writing
    free(empty_page);
    // Updating the total number of pages in the file handler
    file_handler->totalNumPages++;
    return RC_OK;
}



// Function to ensure capacity
RC ensureCapacity(int number_of_pages, SM_FileHandle *file_handler)
{
    // Open the file in append mode
    file = fopen(file_handler->fileName, "a");
    // Checking if the file handler is properly inttialized
    if (!file_handler)
    {
        fclose(file);
        return RC_FILE_HANDLE_NOT_INIT;
    }
    // Checking if the file opening was unsuccessful
    if (!file)
    {
        return RC_FILE_NOT_FOUND;
    }
    // Checking if the specified number of pages is less than zero
    if (number_of_pages < 0)
    {
        fclose(file);
        return RC_INVALID_NUMBER_OF_PAGES;
    }
    // Variable to store the result of appendEmptyBlock operation
    RC appendResult;
    // Checking that the file has at least the specified number of pages
    while (number_of_pages > file_handler->totalNumPages)
    {
        // Appending empty blocks until the desired capacity is reached
        appendResult = appendEmptyBlock(file_handler);
        // Checking if the append operation was not successful
        if (appendResult != RC_OK)
        {
            fclose(file);
            return appendResult;
        }
    }
    // Closing the file
    fclose(file);
    return RC_OK;
}
