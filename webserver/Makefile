CC=gcc
CFLAGS=-Wall -Wextra #-O2 -pedantic
#CHEADERS=$(shell ls *.h)
CFILES=$(shell ls *.c)
OBFILES=$(shell ls *.o)
#HTMLFILES=$(shell ls *.html)
LIBS=-pthread
TARGET=server

all: $(TARGET)

$(TARGET): $(CFILES) $(CHEADERS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

cln:
	-rm -v $(TARGET) $(OBFILES) # $(HTMLFILES) 
#	-rm -v -rf thumbs
