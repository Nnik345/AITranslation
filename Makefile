CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -Isrc/preprocessing/stemmer -fopenmp
CXXFLAGS = -Wall -Wextra -std=c++11 -Iinclude -fopenmp

# Stemmer source files
STEM_SRC = \
    src/preprocessing/stemmer/api.c \
    src/preprocessing/stemmer/stem_UTF_8_hindi.c \
    src/preprocessing/stemmer/stem_UTF_8_tamil.c \
    src/preprocessing/stemmer/utilities.c \
    src/preprocessing/stemmer_wrapper.c

C_SRC = \
    src/core/main.c \
    src/io/csv_reader.c \
    src/io/csv_writer.c \
    src/preprocessing/preprocess.c \
    src/utils/string_utils.c \
    src/utils/score_utils.c \
    src/scoring/bleu.c \
    src/scoring/meteor_wrapper.c \
    $(STEM_SRC)

CXX_SRC = \
    src/scoring/meteor_engine.cpp

OBJ = $(C_SRC:.c=.o) $(CXX_SRC:.cpp=.o)
TARGET = mt_pipeline

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lstdc++ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean