# Assignment - 1

## Storage Manager

### Team Members:
- Abhiram Ravipati                 - aravipati@hawk.iit.edu      - A20539084 
- Divya Kampalli                   - kdivya1@hawk.iit.edu        - A20539479   
- Sai Jayanth Rajamahendravaram    - srajamahendram@hawk.iit.edu - A20539434
- Sri Samhitha Bobba               - sbobba1@hawk.iit.edu        - A20541559

### Contributions
- Abhiram Ravipati                - CreatePageFile(), openPageFile(),readBlock() , destroyPageFile()
- Divya Kampalli                  - GetBlockPos(), writeBlock(), appendEmptyBlock(), ensureCapacity(), closePageFile()
- Sai Jayanth Rajamahendravaram   - writeCurrentBlock(), readCurrentBlock(), readNextBlock()
- Sri Samhitha Bobba              - readFirstBlock(), readLastBlock(), readPreviousBlock()

#### HOW TO EXECUTE THE SCRIPT

1. Open terminal 

2. Run the command: "make"

3. Run the command "test_assign1" 

4. To remove object files run the command "make clean"


## Overview of the Assignment:
The goal of this assignment is to implement a simple storage manager, a module capable of reading blocks from a file on disk
into memory and writing blocks from memory to a file on disk. The storage manager deals with pages (blocks) of fixed size (PAGE SIZE).
It provides methods for creating, opening, and closing files. 


### Storage Manager Functions:
1. initStorageManager():
    - We will initialize storage manager in this function.


2. createPageFile(): 
    - We will check if the file is already created or not 
    - If the file is already created, we will overwrite the file.
    - If the file is not present, it creates a new page file with the given name.
    

3. openPageFile(): 
    - We will open the file using fopen function
    - If the file is not opened, we will return File not Found error. 
    - Else we will open the file and calculate the page size. 


4. closePageFile(): 
    - We will first check if the file handler is initialize or not. If not initialized, return error code. 
    - If file is open, close the file by setting file pointer to null. 


5. destroyPageFile(): 
    - Check if file is present or not. 
    - Deletes the page file with the given name if it is present.
    - If not found, throw file not found error. 


6. readBlock(): 
    - Open the page file in read mode using fopen, storing the file pointer in the 'file' variable.
    - Check if the file is successfully opened. If not, return RC_FILE_NOT_FOUND.
    - Perform various checks to validate the file handler, page number, and file existence.
    - If the page number is out of bounds, close the file and return RC_READ_NON_EXISTING_PAGE.
    - Calculate the absolute position in the file for the requested page.
    - Use fseek to move the file pointer to the calculated position.
    - Check if fseek operation is successful. If not, handle the error based on errno and return the corresponding error code.
    - Update the current page position in the file handler.
    - Use fread to read PAGE_SIZE bytes from the file into the provided pageData buffer.
    - Check if the read operation is successful. If not, close the file and return RC_READING_FAILED.
    - Update the current page position in the file handler to the start of the next page.
    - Close the file and return RC_OK indicating a successful read operation.


7. readFirstBlock(): 
    - Check if the file handler and page data are properly initialized. If not, return RC_FILE_HANDLE_NOT_INIT or RC_PAGE_HANDLE_NOT_INIT accordingly.
    - Set the page number to read as 0, representing the first block.
    - Call the readBlock function with the specified parameters to read the first block.
    - Return the result of the readBlock operation, which can be either RC_OK for a successful read or an error code indicating the failure.


8. readPreviousBlock(): 
    - Check if the file handler and page data are properly initialized. If not, return RC_FILE_HANDLE_NOT_INIT or RC_PAGE_HANDLE_NOT_INIT accordingly.
    - Calculate the current page number based on the current position in the file and the page size.
    - Calculate the page number of the previous block by subtracting 1 from the current page number.
    - Check if the calculated previous page number is within the valid range (greater than or equal to 0 and less than the total number of pages).
    - If the calculated previous page number is outside the valid range, return RC_READ_NON_EXISTING_PAGE.
    - Call the readBlock function with the calculated previous page number and specified parameters to read the previous block.
    - Return the result of the readBlock operation, which can be either RC_OK for a successful read or an error code indicating the failure.


9. readCurrentBlock(): 
    - Check if the file handler and page data are properly initialized. If not, return RC_FILE_HANDLE_NOT_INIT or RC_PAGE_HANDLE_NOT_INIT accordingly.
    - Calculate the current page number based on the current position in the file and the page size.
    - Check if the calculated current page number is within the valid range (greater than or equal to 0 and less than the total number of pages).
    - If the calculated current page number is outside the valid range, return RC_READ_NON_EXISTING_PAGE.
    - Call the readBlock function with the calculated current page number and specified parameters to read the current block.
    - Return the result of the readBlock operation, which can be either RC_OK for a successful read or an error code indicating the failure.



10. readNextBlock(SM_FileHandler *file_handler, SM_PageData pageData): 
    - Check if the file handler and page data are properly initialized. If not, return RC_FILE_HANDLE_NOT_INIT or RC_PAGE_HANDLE_NOT_INIT accordingly.
    - Calculate the current page number based on the current position in the file and the page size.
    - Increment the current page number to get the page number of the next block.
    - Call the readBlock function with the calculated next page number and specified parameters to read the next block.
    - Return the result of the readBlock operation, which can be either RC_OK for a successful read or an error code indicating the failure.



11. readLastBlock(): 
    - Check if the file handler and page data are properly initialized. If not, return RC_FILE_HANDLE_NOT_INIT or RC_PAGE_HANDLE_NOT_INIT accordingly.
    - Calculate the page number of the last block in the file based on the total number of pages.
    - Check if the calculated last block is within the valid range of page numbers.
    - Call the readBlock function with the calculated last page number and specified parameters to read the last block.
    - Return the result of the readBlock operation, which can be either RC_OK for a successful read or an error code indicating the failure.


12. writeBlock(): 
    - Check if the provided page number is valid (non-negative and within the total number of pages).
    - Open the file in read and write mode ("r+").
    - Verify if the file handler, page data, and file stream are properly initialized. Return appropriate error codes if not.
    - Move the file pointer to the beginning of the specified page in the file using fseek.
    - Write the content of the given page data to the file.
    - Update the current page position and total number of pages in the file handler based on the written page.
    - Close the file.
    - Return RC_OK if the write operation is successful; otherwise, return RC_WRITE_FAILED.


13. writeCurrentBlock(): 
    - Open the file in read and write mode ("r+").
    - Check if the file handler and page data are properly initialized. Return appropriate error codes if not.
    - Verify if the file stream is successfully opened. Return RC_FILE_NOT_FOUND if it fails.
    - Get the current position and current page number from the file handler.
    - Set the file pointer to the beginning of the current page using fseek.
    - Write the content of the given page data to the file.
    - Update the current page position in the file handler based on the written page.
    - Close the file.
    - Return RC_OK if the write operation is successful; otherwise, return RC_FILE_NOT_FOUND.


14. appendEmptyBlock(): 
    - Check if the file handler is initialized. Return RC_FILE_HANDLE_NOT_INIT if not.
    - Allocate memory for an empty page of size PAGE_SIZE using calloc.
    - Move the file pointer to the end of the file using fseek.
    - Check if the seek operation is successful. Return RC_WRITE_FAILED if it fails.
    - Write the content of the empty page to the file.
    - Deallocate the memory allocated for the empty page.
    - Increment the total number of pages in the file handler.
    - Return RC_OK if the append operation is successful; otherwise, return RC_WRITE_FAILED.


15. ensureCapacity(): 
    - Open the file in 'a' mode to append data.
    - Check if the file handler is initialized. Return RC_FILE_HANDLE_NOT_INIT if not.
    - Check if the file is successfully opened. Return RC_FILE_NOT_FOUND if not.
    - Check if the number_of_pages is less than 0. Return RC_INVALID_NUMBER_OF_PAGES if true.
    - Declare a variable appendResult to store the result of the appendEmptyBlock function.
    - While the total number of pages in the file is less than the required number_of_pages, repeatedly append an empty block.
    - Check if the append operation is successful. If not, close the file, and return the result from appendEmptyBlock.
    - Close the file and return RC_OK, indicating successful capacity assurance.
