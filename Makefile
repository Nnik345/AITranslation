# Machine Translation Evaluation Pipeline
# Makefile for building the MT evaluation system

CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -Isrc/preprocessing/stemmer -fopenmp
CXXFLAGS = -Wall -Wextra -std=c++11 -Iinclude -fopenmp

# Stemmer source files (Snowball stemmers for Hindi and Tamil)
STEM_SRC = \
    src/preprocessing/stemmer/api.c \
    src/preprocessing/stemmer/stem_UTF_8_hindi.c \
    src/preprocessing/stemmer/stem_UTF_8_tamil.c \
    src/preprocessing/stemmer/utilities.c \
    src/preprocessing/stemmer_wrapper.c

# C source files
C_SRC = \
    src/core/main.c \
    src/io/csv_reader.c \
    src/io/csv_writer.c \
    src/preprocessing/preprocess.c \
    src/utils/string_utils.c \
    src/utils/score_utils.c \
    src/scoring/bleu.c \
    src/scoring/meteor_wrapper.c \
    src/scoring/comet_wrapper.c \
    $(STEM_SRC)

# C++ source files
CXX_SRC = \
    src/scoring/meteor_engine.cpp

# Object files and target
OBJ = $(C_SRC:.c=.o) $(CXX_SRC:.cpp=.o)
TARGET = mt_pipeline

# Default target
all: $(TARGET)

# Link all object files
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lstdc++ -lm

# Compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean