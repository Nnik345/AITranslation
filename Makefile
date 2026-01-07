CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -fopenmp

SRC = \
    src/core/main.c \
    src/io/csv_reader.c \
    src/io/csv_writer.c \
    src/preprocessing/preprocess.c \
    src/utils/string_utils.c

OBJ = $(SRC:.c=.o)
TARGET = mt_pipeline

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean