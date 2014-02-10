SOURCES		:= src/clang-cache.cpp src/client/client.cpp src/server/server.cpp src/utils/mkdirp.cpp src/utils/daemon.cpp
DOCSOURCES	:= doc/clang-cache.1.md

CXX		?= g++
CXXFLAGS	:= -std=c++11 -Wall -Wextra -Werror -I$(dir $(lastword $(MAKEFILE_LIST)))src
LDFLAGS		:=
PREFIX		:= /usr/local

OBJECTS		:= $(SOURCES:.cpp=.o)
DEPENDS		:= $(SOURCES:.cpp=.d)
TARGET		:= src/clang-cache
DOCTARGETS	:= $(DOCSOURCES:.1.md=.1)

VPATH		:= $(dir $(lastword $(MAKEFILE_LIST)))

default: $(TARGET)

all: $(TARGET) $(DOCTARGETS)

install: all
	install -d $(PREFIX)/bin/
	install -d $(PREFIX)/share/man/man1/
	install -s -t $(PREFIX)/bin/ $(TARGET)
	install -t $(PREFIX)/share/man/man1/ $(DOCTARGETS)

clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)

distclean: clean
	rm -f $(TARGET)
	rm -f $(DOCTARGETS)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -o $@ -c $<

%.1: %.1.md
	@mkdir -p $(dir $@)
	ronn --roff --pipe $< >$@

.PHONY: all install clean distclean

-include $(DEPENDS)
