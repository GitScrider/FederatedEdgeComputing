#CFLAGS := -Wall -Wextra -std=c99
CC = gcc
CFLAGS = -Wall -Wextra
SRCDIR = src
INCDIR = lib
BUILDDIR = .build
PROGRAM_NAME = FederatedLearningServer
TARGET = $(PROGRAM_NAME)
LIBS = -lpthread -lwebsockets

SOURCES := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(INCDIR)/*.h)
OBJECTS := $(filter-out $(SRCDIR)/main.c, $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o))


$(TARGET): $(OBJECTS) 
	$(CC) $(CFLAGS) $^ main.c -o $@ $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(INCLUDES)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -f $(BUILDDIR)/*.o $(TARGET)

run:
	./$(PROGRAM_NAME)
