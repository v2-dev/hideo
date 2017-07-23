CFLAGS=-Wall -Wextra -O2
CHEADERS=$(shell ls *.h)
CFILES=$(shell ls *.c)
OBFILES=$(shell ls *.o)
#HTMLFILES=$(shell ls *.html)
LDFLAGS=-pthread -lwurfl
TARGET=server

all: $(TARGET)

$(TARGET): $(CFILES) $(CHEADERS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


debug: $(CFILES) $(CHEADERS)
	$(CC) $(CFLAGS) -ggdb -o $@ $^ $(LDFLAGS)

.PHONY: clean all

clean:
	-$(RM) -v $(TARGET) $(OBFILES) *~ # $(HTMLFILES)
#	-rm -v -rf thumbs
