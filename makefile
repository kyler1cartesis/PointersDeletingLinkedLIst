CC=gcc
CFLAGS=-Wall -std=c11

debug: LinkedList.c LinkedList.h PointersReadingLL.c
	$(CC) LinkedList.c PointersReadingLL.c -o PointersReadingLL $(CFLAGS)
	chmod +x ./PointersReadingLL

release: LinkedList.c LinkedList.h PointersReadingLL.c
	$(CC) -O2 LinkedList.c PointersReadingLL.c -o PointersReadingLL $(CFLAGS)
	chmod +x ./PointersReadingLL