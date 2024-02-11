CC = gcc
CFLAGS = -Wall -Wextra
DEBUGFLAGS = -g
SRCDIR = src
INCDIR = lib
BUILDDIR = .build
PROGRAM_NAME = FederatedLearningServer
TARGET = $(PROGRAM_NAME)
LIBS = -lpthread -lwebsockets -lm

SOURCES := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(INCDIR)/*.h)
OBJECTS := $(filter-out $(SRCDIR)/main.c, $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o))

all: clean build run

alldebug: clean builddebug rundebug

build: $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(CFLAGS) $^ main.c -o $@ $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(INCLUDES)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

builddebug:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(SOURCES) main.c -o $(TARGET) $(LIBS)

clean:
	rm -f $(BUILDDIR)/*.o $(TARGET)

run:
	./$(PROGRAM_NAME)

rundebug:
	gdb ./$(TARGET)