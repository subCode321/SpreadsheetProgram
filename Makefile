CC = gcc
CFLAGS = -g -O3 -Wall -Wextra -std=c11
LDFLAGS = -lm

TARGET_DIR = target/release
TARGET = $(TARGET_DIR)/spreadsheet2023EE31148_2023CS10022_2023CS11148

OBJ_DIR = obj

SRC = 1.c display.c Functions.c Graph.c Parser.c
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))

REPORT_SRC = report.tex
REPORT_PDF = report.pdf

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJ)
	@mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

# Build object files
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run tests
test: $(TARGET)
	./$(TARGET) < input.txt  

# Compile LaTeX report
report: $(REPORT_SRC)
	pdflatex -interaction=nonstopmode -halt-on-error $(REPORT_SRC)
	pdflatex -interaction=nonstopmode -halt-on-error $(REPORT_SRC)

# Cleanup
clean:
	rm -rf $(OBJ_DIR) $(TARGET_DIR) $(REPORT_PDF) *.aux *.log *.out

.PHONY: all test report clean
