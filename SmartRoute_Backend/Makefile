
CC = C:/msys64/ucrt64/bin/gcc.exe

CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -Iinclude

TARGET = bin/smartroute.exe

SOURCES = src/main.c src/data.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) include/models.h include/data.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	del /Q $(TARGET) 2>NUL
