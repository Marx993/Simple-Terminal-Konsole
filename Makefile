# Compiler settings
CXX = g++
# Automatically grab compilation flags for VTE 2.91, GTK3, and C++17
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 $(shell pkg-config --cflags vte-2.91 gtk+-3.0)
LIBS = $(shell pkg-config --libs vte-2.91 gtk+-3.0)

# Target binary name
TARGET = stk

# Target installation directory
PREFIX = /usr/local

all: $(TARGET)

$(TARGET): stk.cpp
	$(CXX) $(CXXFLAGS) stk.cpp -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -D -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)

.PHONY: all clean install uninstall
