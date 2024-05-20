#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "record_mgr.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "const.h"

const int maxNumberOfPages = 100;

const int attributeSize = 15; 

RecordManager *recordManager;

/*
	- Function: initRecordManager
	- Description: Initializes the record manager.
	- Parameters:
		- mgmtData: A pointer to management data (not used in this function).
	- Returns:
		- RC_OK if the record manager is successfully initialized.
*/
extern RC initRecordManager (void *mgmtData)
{
	// Initialize the storage manager
	initStorageManager();
	// Return success status
	return RC_OK;
}

/*
	- Function: shutdownRecordManager
	- Description: Shuts down the record manager, freeing associated resources.
	- Parameters: None.
	- Returns:
		- RC_OK if the record manager is successfully shut down.
*/
extern RC shutdownRecordManager ()
{
	// Clear the record manager pointer
	recordManager = NULL;
	// Free the memory associated with the record manager
	free(recordManager);
	// Return success status
	return RC_OK;
}

/*
	- Function: getFreeSpace
	- Description: Finds the index of the first available slot in a page for storing a record.
	- Parameters:
		- data: Pointer to the data of the page.
		- sizeOfRecord: Size of the record to be stored.
	- Returns:
		- The index of the first available slot if found, otherwise returns -1.
*/
int getFreeSpace(char* data, int sizeOfRecord) {
    int i = 0;
    while (i < PAGE_SIZE / sizeOfRecord) {
		// Check if the slot is available
        if (!(data[i * sizeOfRecord] == '+')) {
			// Return the index of the available slot
            return i;
        }
        i++;
    }
	// Return -1 if no available slot is found
    return -1;
}


/*
	- Function: createTable
	- Description: Creates a new table with the given name and schema.
	- Parameters:
		- tableName: Name of the table to be created.
		- tableSchema: Schema of the table to be created.
	- Returns:
		- RC_OK if the table is successfully created.
*/
extern RC createTable(char *tableName, Schema *tableSchema) {
    // Allocate memory for the record manager
    recordManager = (RecordManager *)malloc(sizeof(RecordManager));
	int i;
    // Initialize buffer pool for the table
    initBufferPool(&recordManager->bufferPool, tableName, maxNumberOfPages, RS_LRU, NULL);

    char pageData[PAGE_SIZE];
    char *ptrPage = pageData;
	// Set number of tuples to 0
    *(int *)ptrPage = 0;
	int result;
    ptrPage += sizeof(int);
	// Set the first free page number to 1
    *(int *)ptrPage = 1;
    ptrPage += sizeof(int);
	// Store the number of attributes in the table schema
    *(int *)ptrPage = tableSchema->numAttr;
    ptrPage += sizeof(int);
	// Store the size of the key
    *(int *)ptrPage = tableSchema->keySize;
    ptrPage += sizeof(int);
	// Store attribute names, data types, and type lengths
    for (i = 0; i < tableSchema->numAttr; i++) {
        // Store attribute names, data types, and type lengths
        strncpy(ptrPage, tableSchema->attrNames[i], attributeSize);
        ptrPage += attributeSize;
		// Store data type of the attribute
        *(int *)ptrPage = (int)tableSchema->dataTypes[i];
        ptrPage += sizeof(int);
		// Store type length of the attribute
        *(int *)ptrPage = (int)tableSchema->typeLength[i];
        ptrPage += sizeof(int);
    }

    SM_FileHandle fileHndl;
	// Create page file if it doesn't exist
    result = (createPageFile(tableName) != RC_OK) ? createPageFile(tableName) : RC_OK;
	// Open page file
    result = (openPageFile(tableName, &fileHndl) != RC_OK) ? openPageFile(tableName, &fileHndl) : RC_OK;
	// Write page data to the first page of the file
    result = (writeBlock(0, &fileHndl, pageData) != RC_OK) ? writeBlock(0, &fileHndl, pageData) : RC_OK;
	// Close page file
    result = (closePageFile(&fileHndl) != RC_OK) ? closePageFile(&fileHndl) : RC_OK;

    return result;
}

/*
	- Function: openTable
	- Description: Opens an existing table and initializes its metadata.
	- Parameters:
		- rel: Pointer to RM_TableData structure where metadata of the table will be stored.
		- name: Name of the table to be opened.
	- Returns:
		- RC_OK if the table is successfully opened and metadata is initialized.
*/
extern RC openTable(RM_TableData *rel, char *name)
{
	
	char *pageHandle = NULL;
	// Set the record manager pointer in RM_TableData to the global record manager instance
	rel->mgmtData = recordManager;
	rel->name = name;
	// Pin the first page of the table to read its metadata
	if (pinPage(&recordManager->bufferPool, &recordManager->pageHandle, 0) != RC_OK)
	{
		return RC_ERROR;
	}
	pageHandle = (char *)recordManager->pageHandle.data;
	// Check if page handle is NULL
	if (pageHandle == NULL)
	{
		
		return RC_ERROR;
	}
	// Read metadata from the first page
	recordManager->tuplesCount = *(int *)pageHandle;

	pageHandle += sizeof(int);

	
	recordManager->freePage = *(int *)pageHandle;
	pageHandle += sizeof(int);

	
	int attrCount = *(int *)pageHandle;
	pageHandle += sizeof(int);
	// Allocate memory for schema
	Schema *schema = (Schema *)malloc(sizeof(Schema));
	if (schema == NULL)
	{
		
		return RC_MEM_ALLOC_FAILED;
	}
	// Allocate memory for attribute names, data types, and type lengths
	schema->numAttr = attrCount,
	schema->typeLength = malloc(sizeof(int) * attrCount),
	schema->attrNames = malloc(sizeof(char *) * attrCount),
	schema->dataTypes = malloc(sizeof(DataType) * attrCount);

	// Check memory allocation
	if (schema->typeLength == NULL || schema->attrNames == NULL || schema->dataTypes == NULL)
	{
		
		if (schema->typeLength != NULL)
			free(schema->typeLength);
		if (schema->attrNames != NULL)
			free(schema->attrNames);
		if (schema->dataTypes != NULL)
			free(schema->dataTypes);
		free(schema);
		return RC_MEM_ALLOC_FAILED;
	}

	if (schema->typeLength == NULL || schema->attrNames == NULL || schema->dataTypes == NULL)
	{
		
		free(schema->typeLength);
		free(schema->attrNames);
		free(schema->dataTypes);
		free(schema);
	}

	// Read attribute names, data types, and type lengths
	for (int i = 0; i < attrCount; i++)
	{
		schema->attrNames[i] = (char *)malloc(attributeSize);
		if (schema->attrNames[i] == NULL)
		{
			
			for (int j = 0; j < i; j++)
			{
				free(schema->attrNames[j]);
			}
			free(schema->typeLength);
			free(schema->attrNames);
			free(schema->dataTypes);
			free(schema);
		}
		strncpy(schema->attrNames[i], pageHandle, attributeSize);
		pageHandle += attributeSize;

		schema->dataTypes[i] = *(int *)pageHandle;
		pageHandle += sizeof(int);

		schema->typeLength[i] = *(int *)pageHandle;
		pageHandle += sizeof(int);
	}

	// Check if schema is not NULL
	if (schema != NULL)
	{
		rel->schema = schema;
	}
	else
	{
		
		return RC_MEM_ALLOC_FAILED;
	}

	// Unpin the page after reading metadata
	unpinPage(&recordManager->bufferPool, &recordManager->pageHandle);

	// Force writing the page to disk
	forcePage(&recordManager->bufferPool, &recordManager->pageHandle);

	return RC_OK;
}


/*
	- Function: getNumTuples
	- Description: Retrieves the number of tuples in the specified table.
	- Parameters:
		- rel: Pointer to RM_TableData structure representing the table.
	- Returns:
		- The number of tuples in the table.
*/
extern int getNumTuples (RM_TableData *rel)
{
	RecordManager *recordManager = rel->mgmtData;
	return recordManager->tuplesCount;
}

/*
	- Function: deleteTable
	- Description: Deletes the table with the specified name from the database.
	- Parameters:
		- name: Name of the table to be deleted.
	- Returns:
		- RC_OK if the table is successfully deleted.
*/
extern RC deleteTable (char *name)
{
	// Delete the page file associated with the table
	destroyPageFile(name);
	// Return success status
	return RC_OK;
}

/*
	- Function: insertRecord
	- Description: Inserts a new record into the specified table.
	- Parameters:
		- rel: Pointer to RM_TableData structure representing the table.
		- record: Pointer to the Record structure containing the data to be inserted.
	- Returns:
		- RC_OK if the record is successfully inserted.
*/
extern RC insertRecord(RM_TableData* rel, Record* record) {
	int rSize ;
    
    RID *rID = &record->id;
	rSize = getRecordSize(rel->schema);
	RecordManager *recordMngr = rel->mgmtData;

   
    rID->page = recordMngr->freePage;
    
    pinPage(&recordMngr->bufferPool, &recordMngr->pageHandle, rID->page);

    
    char* auxPointer = recordMngr->pageHandle.data;

    // Find a free slot in the page to insert the record
    for (; (rID->slot = getFreeSpace(auxPointer, rSize)) == -1;) {
		rID->page++;
        
        unpinPage(&recordMngr->bufferPool, &recordMngr->pageHandle);
        
        pinPage(&recordMngr->bufferPool, &recordMngr->pageHandle, rID->page);

       
        auxPointer = recordMngr->pageHandle.data;
    }
	  
    auxPointer += (rID->slot * rSize);

    
    markDirty(&recordMngr->bufferPool, &recordMngr->pageHandle);

    // Copy record data to the page
    memcpy(auxPointer+1, record->data + 1, rSize - 1);

    // Mark the slot as occupied
    *auxPointer = '+';

    
    unpinPage(&recordMngr->bufferPool, &recordMngr->pageHandle);

    recordMngr->tuplesCount++;

    pinPage(&recordMngr->bufferPool, &recordMngr->pageHandle, 0);

	recordManager->tuplesCount--;
	
    return RC_OK;
}

/*
	- Function: deleteRecord
	- Description: Deletes the record specified by the given RID from the table.
	- Parameters:
		- table: Pointer to RM_TableData structure representing the table.
		- id: RID (Record ID) of the record to be deleted.
	- Returns:
		- RC_OK if the record is successfully deleted.
*/
extern RC deleteRecord(RM_TableData* table, RID id) {
    
    RecordManager *recordMgr = table->mgmtData;
	int recordSize ;

     // Pin the page containing the record
    RC pinPageResult = pinPage(&recordMgr->bufferPool, &recordMgr->pageHandle, id.page);
    // Mark the page as dirty regardless of pinning result
	markDirty(&recordMgr->bufferPool, &recordMgr->pageHandle); // Mark the page as dirty regardless of pinning result

    if (pinPageResult == RC_OK) {
        
        recordSize = getRecordSize(table->schema);
        char* recordDataPtr = recordMgr->pageHandle.data + (id.slot * recordSize);
        // Mark the record as deleted
		*recordDataPtr = '-'; 

       
        if (id.page > recordMgr->freePage) {
            recordMgr->freePage = id.page;
        }

        
        RC unpinPageResult = unpinPage(&recordMgr->bufferPool, &recordMgr->pageHandle);
        if (unpinPageResult != RC_OK) {
            // Return error code if unpinning fails
			return unpinPageResult; 
        }
		// Return success status
        return RC_OK;
    } else {
		// Return pinning error code
        return pinPageResult; 
    }
}


extern RC updateRecord(RM_TableData* rel, Record* record) {   
    
    RecordManager *recordMngr = rel->mgmtData;
	int rSize;

    // Pin the page containing the record
    if (pinPage(&recordMngr->bufferPool, &recordMngr->pageHandle, record->id.page) == RC_OK) {
        // Mark the page as dirty
		markDirty(&recordMngr->bufferPool, &recordMngr->pageHandle);

        
        rSize = getRecordSize(rel->schema);
        char* dataP = recordMngr->pageHandle.data + (record->id.slot * rSize);

        // Mark the record as valid
        *dataP = '+'; 

        // Update the record data
        memcpy(dataP + 1, record->data + 1, rSize - 1);

        // Unpin the page
        if (unpinPage(&recordMngr->bufferPool, &recordMngr->pageHandle) == RC_OK) {
            // Return success status
			return RC_OK;
        } else {
			// Return error code if unpinning fails
            return RC_UNPIN_PAGE_FAILED; 
        }
    } else {
		// Return error code if pinning fails
        return RC_PIN_PAGE_FAILED;
    }
}

/*
	- Function: getRecord
	- Description: Retrieves the record specified by the given RID from the table.
	- Parameters:
		- rel: Pointer to RM_TableData structure representing the table.
		- id: RID (Record ID) of the record to be retrieved.
		- record: Pointer to the Record structure where the retrieved data will be stored.
	- Returns:
		- RC_OK if the record is successfully retrieved.
*/
extern RC getRecord(RM_TableData* rel, RID id, Record* record) {
    
    RecordManager* recordMngr = rel->mgmtData;
	int rSize;
    // Pin the page containing the record
    if (pinPage(&recordMngr->bufferPool, &recordMngr->pageHandle, id.page) != RC_OK) {
        // Return error code if pinning fails
		return RC_PIN_PAGE_FAILED; 
    }

    
    rSize = getRecordSize(rel->schema);
    char* pageData = recordMngr->pageHandle.data + (id.slot * rSize);

    // Check if the record is valid
    if (*pageData != '+') {
        // Unpin the page
        if (unpinPage(&recordMngr->bufferPool, &recordMngr->pageHandle) != RC_OK) {
            // Return error code if unpinning fails
			return RC_UNPIN_PAGE_FAILED; 
        }
        // Return error code if no tuple is found with the given RID
		return RC_RM_NO_TUPLE_WITH_GIVEN_RID;
    }

   
    record->id = id;
    char* data = record->data;
    // Copy record data to the record structure
	memcpy(++data, pageData + 1, rSize - 1);

    // Unpin the page
    if (unpinPage(&recordMngr->bufferPool, &recordMngr->pageHandle) != RC_OK) {
        // Return error code if unpinning fails
		return RC_UNPIN_PAGE_FAILED; 
    }
	// Return success status
    return RC_OK;
}



/*
	- Function: startScan
	- Description: Initializes a scan on the specified table with the given condition.
	- Parameters:
		- rel: Pointer to RM_TableData structure representing the table to be scanned.
		- scan: Pointer to RM_ScanHandle structure where the scan information will be stored.
		- cond: Pointer to Expr structure representing the scan condition.
	- Returns:
		- RC_OK if the scan is successfully initialized.
*/
extern RC startScan(RM_TableData *rel, RM_ScanHandle *scan, Expr *cond) {
    // Check if a scan condition is provided
    if (cond != NULL) {
         // Allocate memory for scan manager
        RecordManager *scanner = (RecordManager*)malloc(sizeof(RecordManager));
        if (scanner == NULL) {
            return RC_MEM_ALLOC_FAILED;
        }
        scanner->recordID.page = 1;
        scanner->recordID.slot = 0;
        scanner->scanCount = 0;
        scanner->condition = cond;
        scan->mgmtData = scanner;
        scan->rel = rel;

          // Open a temporary table for the scan
        if (openTable(rel, "ScanTable") != RC_OK) {
            free(scanner);
            return RC_FILE_NOT_FOUND;
        }
        RecordManager *tableManager = rel->mgmtData;
        tableManager->tuplesCount = attributeSize;
        return RC_OK;
    } else {
        return RC_SCAN_CONDITION_NOT_FOUND;
    }
}

/*
	- Function: closeTable
	- Description: Closes the specified table, releasing associated resources.
	- Parameters:
		- rel: Pointer to RM_TableData structure representing the table to be closed.
	- Returns:
		- RC_OK if the table is successfully closed.
*/
extern RC closeTable (RM_TableData *rel)
{
	RecordManager *recordManager = (*rel).mgmtData;
	// Shutdown the buffer pool associated with the table
	shutdownBufferPool(&recordManager->bufferPool);

	return RC_OK;
}


/*
	- Function: next
	- Description: Retrieves the next record in the scan result set.
	- Parameters:
		- scan: Pointer to RM_ScanHandle structure representing the scan.
		- record: Pointer to Record structure where the retrieved record will be stored.
	- Returns:
		- RC_OK if the next record is successfully retrieved.
*/
extern RC next(RM_ScanHandle *scan, Record *record) {
    RecordManager *tableManager = scan->rel->mgmtData;
    RecordManager *scanMgr = scan->mgmtData;
	if (!scanMgr->condition) {
        // Return error code if scan condition is not found
		return RC_SCAN_CONDITION_NOT_FOUND;
    }
    Schema *schema = scan->rel->schema;

    

    Value *result = (Value *) malloc(sizeof(Value));
    char *data;

    int sizeOfRecord = getRecordSize(schema);
    int totalSlots = PAGE_SIZE / sizeOfRecord;
    if (!totalSlots) {
        return RC_RM_NO_MORE_SLOTS;
    }
    int scanCount = scanMgr->scanCount;
    int tuplesCount = tableManager->tuplesCount;

    if (!tuplesCount) {
        return RC_RM_NO_MORE_TUPLES;
    }

    while (scanCount < tuplesCount) {
        (scanCount <= 0) ? (scanMgr->recordID.page = 1, scanMgr->recordID.slot = 0) : (++scanMgr->recordID.slot >= totalSlots ? (scanMgr->recordID.slot = 0, ++scanMgr->recordID.page) : 0);

        pinPage(&tableManager->bufferPool, &scanMgr->pageHandle, scanMgr->recordID.page);
        data = scanMgr->pageHandle.data;
        data += (scanMgr->recordID.slot * sizeOfRecord);
		 char *dataPointer = record->data;

        record->id.slot = scanMgr->recordID.slot;
		        record->id.page = scanMgr->recordID.page;

       
		if (!dataPointer|| !record){
            return RC_ERROR;
        }
        // Mark the record as read
        *dataPointer = '-'; 
		dataPointer++;
        memcpy(dataPointer, data + 1, sizeOfRecord - 1);

        scanMgr->scanCount++;

        evalExpr(record, schema, scanMgr->condition, &result);
		scanCount++;

        if (result->v.boolV) {
            unpinPage(&tableManager->bufferPool, &scanMgr->pageHandle);
            // Return success status
			return RC_OK;
        }
    }

    unpinPage(&tableManager->bufferPool, &scanMgr->pageHandle);
	scanMgr->freePage = 0;
	scanMgr->recordID.slot = 0;
	scanMgr->scanCount = 0;
    scanMgr->recordID.page = 1;

    return RC_RM_NO_MORE_TUPLES;
}


/*
	- Function: closeScan
	- Description: Closes the specified scan, releasing associated resources.
	- Parameters:
		- scan: Pointer to RM_ScanHandle structure representing the scan to be closed.
	- Returns:
		- RC_OK if the scan is successfully closed.
*/
extern RC closeScan(RM_ScanHandle *scan) {
    // Check if scan handle or its management data is NULL
    if (scan == NULL || scan->mgmtData == NULL) {
        // Return error code if scan handle or its management data is NULL
		return RC_ERROR;
    }
    
   
    RecordManager *scanManager = scan->mgmtData;
    RecordManager *recordManager = scan->rel->mgmtData;

    // If scan has been started
    if (scanManager->scanCount > 0) {
        unpinPage(&recordManager->bufferPool, &scanManager->pageHandle);
		if(!scanManager){
			return RC_ERROR;

		}
        scanManager->recordID.page = 1;
        scanManager->recordID.slot = 0;
        scanManager->scanCount = 0;
    }
	if(!scanManager){
		return RC_ERROR;
	}
    
    // Free memory allocated for scan management data
    free(scan->mgmtData);
    scan->mgmtData = NULL;

    return RC_OK;
}

/*
	- Function: getRecordSize
	- Description: Computes the size of a record based on the schema.
	- Parameters:
		- schema: Pointer to Schema structure representing the schema of the record.
	- Returns:
		- The size of the record.
*/
extern int getRecordSize(Schema *schema) {
    int length = 0;

    
    for (int i = 0; i < schema->numAttr; i++) {
        
        switch (schema->dataTypes[i]) {
            case DT_STRING:
                length += schema->typeLength[i];
                break;
            case DT_INT:
                length += sizeof(int);
                break;
            case DT_FLOAT:
                length += sizeof(float);
                break;
            case DT_BOOL:
                length += sizeof(bool);
                break;
            default:
                
                break;
        }
    }
    return length;
}


/*
	- Function: createSchema
	- Description: Creates a schema structure based on the provided attributes, data types, and key information.
	- Parameters:
		- numAttr: Number of attributes in the schema.
		- attrNames: Array of strings representing attribute names.
		- dataTypes: Array of DataType enum representing data types for each attribute.
		- typeLength: Array of integers representing length of each attribute (only applicable for string type).
		- keySize: Number of key attributes.
		- keys: Array of integers representing key attributes.
	- Returns:
		- Pointer to the created Schema structure.
*/
extern Schema *createSchema(int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys) {
	int i;
	int j;
    // Allocate memory for the schema structure
    Schema *tempSchema = (Schema *)malloc(sizeof(Schema));
    if (tempSchema == NULL) {
        // Return NULL if memory allocation fails
        return NULL;
    }
    
	// Allocate memory for attribute names array
    tempSchema->attrNames = (char **)malloc(numAttr * sizeof(char *));
    if (tempSchema->attrNames == NULL) {
        
        free(tempSchema);
		// Return NULL if memory allocation fails
        return NULL;
    }
	// Copy attribute names
    for (i = 0; i < numAttr; i++) {
        tempSchema->attrNames[i] = strdup(attrNames[i]);
        if (tempSchema->attrNames[i] == NULL) {
            
            for (j = 0; j < i; j++) {
                free(tempSchema->attrNames[j]);
            }
            free(tempSchema->attrNames);
            free(tempSchema);
            return NULL;
        }
    }
    // Assign other attributes of the schema
    tempSchema->numAttr = numAttr;
    tempSchema->dataTypes = dataTypes;
    tempSchema->typeLength = typeLength;
    tempSchema->keySize = keySize;
    tempSchema->keyAttrs = keys;

    return tempSchema;
}


/*
	- Function: freeSchema
	- Description: Frees the memory allocated for the given schema structure.
	- Parameters:
		- schema: Pointer to the Schema structure to be freed.
	- Returns:
		- RC_OK if the schema structure is successfully freed.
*/
extern RC freeSchema (Schema *schema)
{
	// Free memory allocated for the schema structure
	free(schema);
	// Return success status
	return RC_OK;
}

/*
	- Function: createRecord
	- Description: Creates a new record based on the provided schema.
	- Parameters:
		- record: Pointer to a pointer to Record structure where the newly created record will be stored.
		- schema: Pointer to Schema structure representing the schema of the record.
	- Returns:
		- RC_OK if the record is successfully created.
*/
extern RC createRecord(Record **record, Schema *schema) {
    // Calculate the size of the record based on the schema
    int sizeOfRecord = getRecordSize(schema);
	// Allocate memory for the new record
    Record *newRecord = (Record*) malloc(sizeof(Record));
    newRecord->data = (char*) malloc(sizeOfRecord);
    newRecord->id.page = newRecord->id.slot = -1;

    // Mark the record as not read
    *(newRecord->data) = '-';

	// Ensure the record data is null-terminated
    *(newRecord->data + 1) = '\0';

    
    *record = newRecord;

    return RC_OK;
}

/*
	- Function: attrOffset
	- Description: Sets the offset from the initial position to the specified attribute of the record into the 'result' parameter.
	- Parameters:
		- schema: Pointer to Schema structure representing the schema of the record.
		- attrNum: Number of attributes.
		- result: Pointer to an integer where the offset value will be stored.
	- Returns:
		- RC_OK if the offset is successfully calculated.
*/
RC attrOffset(Schema *schema, int attrNum, int *result) {
	    *result = 1;

    int i = 0;
    // Iterate through the attributes
    while (i < attrNum) {
        switch (schema->dataTypes[i]) {
            case DT_STRING: {
                // Increment the offset by the length of the string attribute
                *result += schema->typeLength[i];
                break;
            }
            case DT_INT: {
                // Increment the offset by the size of an integer
                *result += sizeof(int);
                break;
            }
            case DT_FLOAT: {
                // Increment the offset by the size of a float
                *result += sizeof(float);
                break;
            }
            case DT_BOOL: {
                // Increment the offset by the size of a boolean
                *result += sizeof(bool);
                break;
            }
            default: {
				// Handle unknown data types
                break;
            }
        }
        i++;
    }
    return RC_OK;
}

/*
	- Function: getAttr
	- Description: Retrieves the value of a specified attribute from the record and stores it in the provided Value structure.
	- Parameters:
		- record: Pointer to Record structure representing the record from which the attribute value is to be retrieved.
		- schema: Pointer to Schema structure representing the schema of the record.
		- attrNum: Index of the attribute whose value is to be retrieved.
		- value: Pointer to a pointer to Value structure where the attribute value will be stored.
	- Returns:
		- RC_OK if the attribute value is successfully retrieved.
*/
extern RC getAttr(Record *record, Schema *schema, int attrNum, Value **value) {
    // Variable to store the offset of the attribute within the record data
	int varOffset = 0;
    Value *attr = malloc(sizeof(Value));

    
    char *PointerOfData;
    PointerOfData = record->data;
	// Calculate the offset of the attribute
    attrOffset(schema, attrNum, &varOffset);


    
    PointerOfData += varOffset;

    if (attrNum == 1) {
        schema->dataTypes[attrNum] = 1;
    }

    // Retrieve the value based on the attribute's data type
    switch (schema->dataTypes[attrNum]) {
        case DT_STRING: {
            
            int len = schema->typeLength[attrNum];
            attr->v.stringV = (char *) malloc(len + 1);
			if(!len){
				return RC_ERROR;
			}
            
            memcpy(attr->v.stringV, PointerOfData, len);
            attr->v.stringV[len] = '\0';
            attr->dt = DT_STRING;
            break;
        }
        case DT_INT: {
            
            int val = 0;
            memcpy(&val, PointerOfData, sizeof(int));
            attr->v.intV = val;
            attr->dt = DT_INT;
            break;
        }
        case DT_FLOAT: {
           
            float val;
            memcpy(&val, PointerOfData, sizeof(float));
            attr->v.floatV = val;
            attr->dt = DT_FLOAT;
            break;
        }
        case DT_BOOL: {
            
            bool val;
            memcpy(&val, PointerOfData, sizeof(bool));
            attr->v.boolV = val;
            attr->dt = DT_BOOL;
            break;
        }
        default: {
            printf("For the given data type no serializer\n");
            break;
        }
    }

    *value = attr;

    return RC_OK;
}

/*
	- Function: setAttr
	- Description: Sets the value of a specified attribute in the record based on the provided Value structure.
	- Parameters:
		- record: Pointer to Record structure representing the record in which the attribute value is to be set.
		- schema: Pointer to Schema structure representing the schema of the record.
		- attrNum: Index of the attribute whose value is to be set.
		- value: Pointer to Value structure containing the new value of the attribute.
	- Returns:
		- RC_OK if the attribute value is successfully set.
*/
extern RC setAttr(Record *record, Schema *schema, int attrNum, Value *value) {
    int varOffset = 0;

	// Pointer to the beginning of the record data
    char *pointerOfData = record->data;
	 // Calculate the offset of the attribute within the record data
    attrOffset(schema, attrNum, &varOffset);
	// Move the pointer to the attribute's position within the record data
    pointerOfData += varOffset;
	// Set the value of the attribute based on its data type
    switch (schema->dataTypes[attrNum]) {
        case DT_STRING: {
			// Set string value
            int len = schema->typeLength[attrNum];
            strncpy(pointerOfData, value->v.stringV, len);
            pointerOfData += len;
            break;
        }
        case DT_INT: {
			// Set integer value
            *(int *)pointerOfData = value->v.intV;
            pointerOfData += sizeof(int);
            break;
        }
        case DT_FLOAT: {
			// Set float value
            *(float *)pointerOfData = value->v.floatV;
            pointerOfData += sizeof(float);
            break;
        }
        case DT_BOOL: {
			// Set boolean value
            *(bool *)pointerOfData = value->v.boolV;
            pointerOfData += sizeof(bool);
            break;
        }
        default: {
            printf("No Serializer for the defined data type.\n");
            break;
        }
    }

    return RC_OK;
}


/*
	- Function: freeRecord
	- Description: Frees the memory allocated for the given record.
	- Parameters:
		- record: Pointer to the Record structure to be freed.
	- Returns:
		- RC_OK if the record is successfully freed.
*/
extern RC freeRecord (Record *record)
{
	// Free memory allocated for the record
	free(record);
	// Return success status
	return RC_OK;
}