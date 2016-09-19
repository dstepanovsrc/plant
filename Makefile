CC=gcc
CFLAGS=-g -Wall -Werror -O0

BUILDDIR=build
BIN=plant
TESTPYTHON=python
TESTRUN=test/start_test.py
TESTDIR=test/

SOURCES=plant.c config.c plan.c project.c task.c database.c timestamp.c

OBJ=$(SOURCES:%.c=$(BUILDDIR)/%.o)

DEP = $(OBJ:%.o=%.d)

all: builddir $(BUILDDIR)/$(BIN)

builddir:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/$(BIN): $(OBJ)
	$(CC) $^ -o $@

-include $(DEP)

$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

test:
	$(TESTPYTHON) $(TESTRUN) $(TESTDIR)

clean:
	rm -rf $(BUILDDIR)

.PHONY: test clean
