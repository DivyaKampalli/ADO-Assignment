CC = gcc

test_assign1: storage_mgr.o dberror.o test_assign1_1.o
	$(CC) storage_mgr.o dberror.o test_assign1_1.o -o test_assign1.exe

storage_mgr.o: storage_mgr.c storage_mgr.h
	$(CC) -c storage_mgr.c

dberror.o: dberror.c dberror.h
	$(CC) -c dberror.c

test_assign1_1.o: test_assign1_1.c
	$(CC) -c test_assign1_1.c

clean:
	rm *.o test_assign1.exe
