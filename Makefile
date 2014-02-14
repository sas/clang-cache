CXX_SOURCES	:= src/clang-cache.cpp src/client/client.cpp src/server/server.cpp src/utils/daemon.cpp src/utils/logger.cpp src/utils/mkdirp.cpp src/utils/socket.cpp
THRIFT_SOURCES	:= src/proto.thrift
DOC_SOURCES	:= doc/clang-cache.1.md

CXX		?= g++
CXXFLAGS	:= `pkg-config --cflags thrift` -std=c++11 -Wall -Wextra -Werror -I$(dir $(lastword $(MAKEFILE_LIST)))src
LDFLAGS		:= `pkg-config --libs thrift`
PREFIX		:= /usr/local

GEN_SOURCES	:= $(THRIFT_SOURCES:.thrift=_types.cpp) $(THRIFT_SOURCES:.thrift=_constants.cpp)
GEN_HEADERS	:= $(THRIFT_SOURCES:.thrift=_types.h) $(THRIFT_SOURCES:.thrift=_constants.h)
OBJECTS		:= $(CXX_SOURCES:.cpp=.o) $(GEN_SOURCES:.cpp=.o)
DEPENDS		:= $(OBJECTS:.o=.d)
TARGET		:= src/clang-cache
DOCTARGETS	:= $(DOC_SOURCES:.1.md=.1)

VPATH		:= $(dir $(lastword $(MAKEFILE_LIST)))

default: $(TARGET)

all: $(TARGET) $(DOCTARGETS)

install: all
	install -d $(PREFIX)/bin/
	install -d $(PREFIX)/share/man/man1/
	install -s -t $(PREFIX)/bin/ $(TARGET)
	install -t $(PREFIX)/share/man/man1/ $(DOCTARGETS)

clean:
	rm -f $(GEN_SOURCES) $(GEN_HEADERS)
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)

distclean: clean
	rm -f $(TARGET)
	rm -f $(DOCTARGETS)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -o $@ -c $<

%_types.cpp %_types.h %_constants.cpp %_constants.h: %.thrift
	@mkdir -p $(dir $@)
	thrift --gen cpp -out $(dir $@) $<

%.1: %.1.md
	@mkdir -p $(dir $@)
	ronn --roff --pipe $< >$@

.PHONY: all install clean distclean
.SECONDARY:

-include $(DEPENDS)
