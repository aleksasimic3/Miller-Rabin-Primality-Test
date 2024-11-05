CC = g++
LIBS = -lmpfr -lgmpxx -lgmp
INCLUDE = -Iinclude
CFLAGS = -Wall -O3

SOURCE_DIR = src/
INCLUDE_DIR = include/
BINARY_DIR = bin/
TEST_DIR = test/

isPrime.exe: main.o millerRabin.o millerRabinMP.o
	$(CC) -o $(BINARY_DIR)isPrime.exe main.o millerRabin.o millerRabinMP.o $(CFLAGS) $(LIBS) -static

test.exe: test.o millerRabin.o millerRabinMP.o
	$(CC) -o $(BINARY_DIR)test.exe test.o millerRabin.o millerRabinMP.o $(CFLAGS) $(LIBS) -static

main.o: $(SOURCE_DIR)main.cpp $(INCLUDE_DIR)millerRabin.h $(INCLUDE_DIR)millerRabinMP.h
	$(CC) -c -o main.o $(SOURCE_DIR)main.cpp  $(CFLAGS) $(INCLUDE)

millerRabin.o: $(SOURCE_DIR)millerRabin.cpp $(INCLUDE_DIR)millerRabin.h
	$(CC) -c -o millerRabin.o $(SOURCE_DIR)millerRabin.cpp $(CFLAGS) $(INCLUDE)

millerRabinMP.o: $(SOURCE_DIR)millerRabinMP.cpp $(INCLUDE_DIR)millerRabinMP.h $(INCLUDE_DIR)millerRabin.h
	$(CC) -c -o millerRabinMP.o $(SOURCE_DIR)millerRabinMP.cpp  $(CFLAGS) $(INCLUDE)

test.o: $(TEST_DIR)test.cpp $(INCLUDE_DIR)millerRabin.h $(INCLUDE_DIR)millerRabinMP.h
	$(CC) -c -o test.o $(TEST_DIR)test.cpp $(CFLAGS) $(INCLUDE)

.PHONY: clean
clean:
	rm main.o
	rm millerRabin.o
	rm millerRabinMP.o
	rm test.o