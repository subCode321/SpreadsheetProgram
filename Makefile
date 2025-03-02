CC = gcc
CFLAGS = -g -O3 -w -Wall -Wextra -std=c11
CFLAGS2 = -g -w -Wall -Wextra -std=c11
LDFLAGS = -lm

TARGET_DIR = target/release
TARGET = $(TARGET_DIR)/spreadsheet

OBJ_DIR = obj
TEST_SRC = Tests.c
TEST_EXEC = test

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

test:
	$(CC) $(CFLAGS2) $(TEST_SRC) -o $(TEST_EXEC) $(LDFLAGS)
	./$(TEST_EXEC)

report: $(REPORT_SRC)
	pdflatex $(REPORT_SRC) && pdflatex $(REPORT_SRC)

clean:
	rm -f $(OBJ) $(TARGET) $(REPORT_PDF) $(TEST_EXEC) *.aux *.log *.out $(TEST_OBJ)
.PHONY: all test report clean
