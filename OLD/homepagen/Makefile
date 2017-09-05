CC=gcc
CFLAGS=-Wall -Wextra -O2
CHEADERS=$(shell ls *.h)
CFILES=$(shell ls *.c)
OBFILES=$(shell ls *.o)
HTMLFILES=$(shell ls *.html)
#LIBS=-pthread
TARGET=hpg

all: $(TARGET)

$(TARGET): $(CFILES) $(CHEADERS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	-rm -v $(TARGET) $(OBFILES) $(HTMLFILES) 
	-rm -v -rf thumbs
	-rm -v -rf homepage
