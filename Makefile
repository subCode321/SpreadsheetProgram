CC = gcc
CFLAGS = -g -O0 -Wall -Wextra -std=c11
LDFLAGS = -lm

TARGET_DIR = target/release
TARGET = $(TARGET_DIR)/spreadsheet2023EE31148_2023CS11148_2023CS11148

OBJ_DIR = obj

SRC = 1.c display.c Functions.c Graph.c Parser.c
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))

REPORT_SRC = report.tex
REPORT_PDF = report.pdf

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	./$(TARGET) < input.txt  

report: $(REPORT_SRC)
	pdflatex $(REPORT_SRC) && pdflatex $(REPORT_SRC)

clean:
	rm -f $(OBJ) $(TARGET) $(REPORT_PDF) *.aux *.log *.out

.PHONY: all test report clean
