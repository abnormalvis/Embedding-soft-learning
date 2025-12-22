CC := gcc
CFLAGS := -Wall -Wextra -Iinclude -g
SRCDIR := source
BINDIR := build
TARGET := $(BINDIR)/student_ui_test

SRCS := $(SRCDIR)/main_test.c $(SRCDIR)/ui_display.c $(SRCDIR)/stubs.c
OBJS := $(patsubst $(SRCDIR)/%.c,$(BINDIR)/%.o,$(SRCS))

.PHONY: all clean run

all: $(TARGET)

$(BINDIR):
	mkdir -p $(BINDIR)

$(BINDIR)/%.o: $(SRCDIR)/%.c | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@

run: all
	$(TARGET)

clean:
	rm -rf $(BINDIR)
