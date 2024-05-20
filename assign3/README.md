# Assignment - 3

## Record Manager

### Team Members:
- Abhiram Ravipati                 - aravipati@hawk.iit.edu      - A20539084 
- Divya Kampalli                   - kdivya1@hawk.iit.edu        - A20539479   
- Sai Jayanth Rajamahendravaram    - srajamahendram@hawk.iit.edu - A20539434
- Sri Samhitha Bobba               - sbobba1@hawk.iit.edu        - A20541559

### Contributions
- Abhiram Ravipati                - initRecordManager(), shutdownRecordManager(), createTable(), openTable(), createRecord()
- Divya Kampalli                  - closeTable(), deleteTable(), getNumTuples(), insertRecord(), deleteRecord(), updateRecord(), getRecord(),freeRecord()
- Sai Jayanth Rajamahendravaram   - startScan(), next(), getRecordSize(), createSchema(), freeSchema(), 
- Sri Samhitha Bobba              - getAttr(), setAttr()

#### HOW TO EXECUTE THE SCRIPT

1. Open terminal 

2. Run the command: "make"

3. Run the command:"./test_assign3" (For MAC and Linux), "test_assign3" (For Windows)

4. Run the command: "make test_expr"

4. Run the command: "./test_expr" (For MAC and Linux), "test_expr" (For Windows)

4. To remove object files run the command "make clean"

Note: Change rm to del for make clean in make file for windows. 

## Overview of the Assignment:


### Record Manager Functions:
1. initRecordManager():
    - Initializes the record manager by calling the initStorageManager function.
    - Returns RC_OK if the initialization is successful.
2. shutdownRecordManager():
    - Clears the record manager pointer and frees associated memory.
    - Returns RC_OK if the record manager is successfully shut down.
3. createTable():
    - Creates a new table with the given name and schema.
    - Initializes buffer pool for the table and writes page data to the first page of the file.
4. openTable():
    - Opens an existing table and initializes its metadata.
    - Pins the first page of the table to read its metadata and allocates memory for the schema.
5. closeTable():
    - Closes the table identified by the given table handle, releasing any resources associated with it.
    - Ensures that all dirty pages related to the table are written to disk before closing.
    - Returns RC_OK if the table is successfully closed and all resources are released.
6. deleteTable():
    - Deletes the table and all its associated data from the database.
    - Removes the table's metadata and frees up any resources occupied by the table.
    - Ensures that any indexes, constraints, or triggers associated with the table are also removed.
    - Returns RC_OK if the table is successfully deleted from the database.
7. getNumTuples():
    - This function returns the number of tuples (records) in the data structure.
8. insertRecord():
    - Inserts a new record into the data structure.
9. deleteRecord():
    - Deletes a record from the data structure.
10. updateRecord():
    - Updates an existing record in the data structure.
11. getRecord():
    - Retrieves a specific record from the data structure based on some criteria.
12. startScan():
    - Initializes a scan operation to traverse records in the data structure.
13. next():
    - Retrieves the next record in the scan operation.
14. closeScan():
    - Finalizes and closes the scan operation.
15. getRecordSize():
    - Returns the size of a record in the data structure.
16. createSchema():
    - Creates a schema (structure) for the records in the data structure.
17. freeSchema():
    - Frees the memory allocated for a schema.
18. createRecord():
    - Creates a new record with the specified attributes.
19. freeRecord():
    - Frees the memory allocated for a record.
20. getAttr():
    - Retrieves the value of a specific attribute from a record.
21. setAttr():
    - Retrieves the value of a specific attribute from a record.
