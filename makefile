CC = gcc

CFLAGS = -Wall -pthread

TARGET = mergesort

SOURCES = mergesort.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) *.o

