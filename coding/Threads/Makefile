# Alle .c-Dateien im aktuellen Ordner finden
SRCS := $(wildcard *.c)
PROGS := $(SRCS:.c=)

CC := gcc
CFLAGS := -std=c11 -O2 -w -D_GNU_SOURCE -pthread
LDFLAGS :=
LDLIBS  := -pthread

GREEN := \033[0;32m
RESET := \033[0m

.PHONY: all clean list

all: $(PROGS)

%: %.c
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)
	@echo "$(GREEN)Compiled $< ✅$(RESET)"

list:
	@echo "Sources : $(SRCS)"
	@echo "Targets : $(PROGS)"

clean:
	@echo "Cleaning..."
	@$(RM) $(PROGS)
