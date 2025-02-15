
CC = gcc
CFLAGS = -g -O0 -Wall
LDFLAGS = -lm

TARGET = spreadsheet

SRC = 1.c display.c Functions.c Graph.c Parser.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
