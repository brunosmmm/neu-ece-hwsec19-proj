
PREFIX ?=
CC ?= $(PREFIX)gcc
SRCPATH = src
TESTPATH = tests
INCLUDES = include
INCLUDEFLAGS = -I$(INCLUDES)
CFLAGS ?= -Wall -g $(INCLUDEFLAGS)

all: tests liblightc.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

simon_test_basic: $(TESTPATH)/test_simon.o $(SRCPATH)/simon.o
	$(CC) $(CFLAGS) -o $@ $^

speck_test_basic: $(TESTPATH)/test_speck.o $(SRCPATH)/speck.o
	$(CC) $(CFLAGS) -o $@ $^

tests_simon: simon_test_basic
tests_speck: speck_test_basic
tests: tests_simon tests_speck

liblightc.a : $(SRCPATH)/simon.o $(SRCPATH)/speck.o
	$(PREFIX)ar rcs $@ $^

clean:
	rm -rf $(TESTPATH)/*.o $(SRCPATH)/*.o $(TESTPATH)/speck_test_basic $(TESTPATH)/simon_test_basic liblightc.a

.PHONY: clean
