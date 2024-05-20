CC = gcc

all: run1 run2

run1: test_assign4_1.o btree_mgr.o rm_serializer.o record_mgr.o dberror.o storage_mgr.o buffer_mgr.o expr.o
	$(CC) -o test_assign4 test_assign4_1.o btree_mgr.o rm_serializer.o record_mgr.o dberror.o storage_mgr.o buffer_mgr.o expr.o -lm

run2: test_expr.o btree_mgr.o rm_serializer.o record_mgr.o dberror.o storage_mgr.o buffer_mgr.o expr.o
	$(CC) -o test_expr test_expr.o btree_mgr.o rm_serializer.o record_mgr.o dberror.o storage_mgr.o buffer_mgr.o expr.o -lm

test_expr.o: test_expr.c dberror.h expr.h record_mgr.h tables.h test_helper.h btree_mgr.h
	$(CC) -c test_expr.c -o test_expr.o -w

test_assign4_1.o: test_assign4_1.c btree_mgr.h dberror.h expr.h record_mgr.h tables.h test_helper.h btree_mgr.h
	$(CC) -c test_assign4_1.c -o test_assign4_1.o -w

rm_serializer.o: dberror.h record_mgr.h tables.h
	$(CC) -c rm_serializer.c -o rm_serializer.o -w

record_mgr.o: record_mgr.c dberror.h storage_mgr.h buffer_mgr.h record_mgr.h tables.h  expr.h
	$(CC) -c record_mgr.c -o record_mgr.o -w

expr.o: expr.c dberror.h expr.h tables.h record_mgr.h
	$(CC) -c expr.c -o expr.o -w

buffer_mgr_stat.o: buffer_mgr_stat.c buffer_mgr_stat.h buffer_mgr.h
	$(CC) -c buffer_mgr_stat.c -o buffer_mgr_stat.o -w

buffer_mgr.o: buffer_mgr.c buffer_mgr.h storage_mgr.h dberror.h  dt.h
	$(CC) -c buffer_mgr.c -o buffer_mgr.o -w

btree_mgr.o: btree_mgr.c btree_mgr.h buffer_mgr.h storage_mgr.h dberror.h  dt.h
	$(CC) -c btree_mgr.c -o btree_mgr.o -w

storage_mgr.o: storage_mgr.c storage_mgr.h dberror.h
	$(CC) -c storage_mgr.c -o storage_mgr.o -w

dberror.o: dberror.c dberror.h
	$(CC) -c dberror.c -o dberror.o -w

clean:
	-rm *.o test_assign4 test_expr
