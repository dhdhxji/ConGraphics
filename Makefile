CC=g++
CFLAGS=-c -Wall
LDFLAGS=
INCLUDE=-Iinclude -Isrc
SOURCES=main.cpp src/console_canvas.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=cube

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@
