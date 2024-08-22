CC=g++
CFLAGS=-I. -Werror -Wpedantic -Wall -Wextra -Ofast -flto -march=native

SRC_DIR=src/
SRCS=$(wildcard $(SRC_DIR)*.cpp)
OBJS=$(SRCS:.cpp=.o)
TARGET=runme

TEST_DIR=test/
DEFAULT_TESTCASE=$(TEST_DIR)/big.txt
TESTCASES = $(DEFAULT_TESTCASE) $(TEST_DIR)/orig.txt $(TEST_DIR)/snake.txt

# The main target
all: cpplint $(TARGET)

# Target to build the executable
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

again: clean $(TARGET) test

hyperfine: $(TARGET)
	hyperfine "./$(TARGET) $(DEFAULT_TESTCASE)"

test: $(TARGET)
	for testcase in $(TESTCASES); do \
        hyperfine --warmup 250 --runs 3000 "./$(TARGET) $$testcase"; \
        ./$(TARGET) $$testcase; \
    done

clean:
	rm -rf $(TARGET) $(OBJS)

cpplint:
	cpplint --filter=-legal,-build,-readability/casting $(wildcard $**/*.[cpp,h])

.PHONY: all $(TARGET) again hyperfine test clean cpplint