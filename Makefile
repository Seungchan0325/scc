# Makefile for SCC project

CC = gcc
CFLAGS = -Wall -g -Iinclude -I$(BUILDDIR)
LDFLAGS = -lfl

SRCDIR = src
BUILDDIR = build
BINDIR = bin

YACC_SRC = $(SRCDIR)/parser.y
YACC_C = $(BUILDDIR)/y.tab.c
YACC_H = $(BUILDDIR)/y.tab.h
LEX_SRC = $(SRCDIR)/lexer.l
LEX_C = $(BUILDDIR)/lex.yy.c

SRC = $(SRCDIR)/main.c $(SRCDIR)/syntax.c $(SRCDIR)/printAST.c
OBJ = $(BUILDDIR)/main.o \
	$(BUILDDIR)/syntax.o \
	$(BUILDDIR)/printAST.o \
	$(BUILDDIR)/y.tab.o \
	$(BUILDDIR)/lexer.o

.PHONY: all clean directories

all: $(BINDIR)/scc

# Create necessary directories
directories:
	mkdir -p $(BUILDDIR) $(BINDIR)

# Link objects into executable
$(BINDIR)/scc: $(OBJ) | directories
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Generate parser
$(YACC_C) $(YACC_H): $(YACC_SRC) | directories
	bison -d -o $(YACC_C) $(YACC_SRC)

# Generate lexer
$(LEX_C): $(LEX_SRC) $(YACC_H) | directories
	flex -o $(LEX_C) $(LEX_SRC)

# Compile C source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(BINDIR)
