CXX_SOURCES	:= src/daemon/clang-cache.cpp		\
		   src/daemon/client/client.cpp		\
		   src/daemon/server/cache.cpp		\
		   src/daemon/server/clc_service.cpp	\
		   src/daemon/server/server.cpp		\
		   src/daemon/utils/abspath.cpp		\
		   src/daemon/utils/cwd.cpp		\
		   src/daemon/utils/daemon.cpp		\
		   src/daemon/utils/logger.cpp		\
		   src/daemon/utils/mkdirp.cpp		\
		   src/daemon/utils/waitdir.cpp

THRIFT_SOURCES	:= src/clc_if.thrift
DOC_SOURCES	:= doc/clang-cache.1.md

CXX		?= g++
CXXFLAGS	:= `pkg-config --cflags thrift` -std=c++11 -pedantic -Wall -Wextra -Werror -I$(dir $(lastword $(MAKEFILE_LIST)))src/daemon -Isrc/daemon -I$(dir $(lastword $(MAKEFILE_LIST)))src -Isrc
LDFLAGS		:= `pkg-config --libs thrift` -lclang
PREFIX		:= /usr/local

GEN_SOURCES	:= $(THRIFT_SOURCES:.thrift=_types.cpp) $(THRIFT_SOURCES:.thrift=_constants.cpp) $(THRIFT_SOURCES:.thrift=.cpp)
GEN_HEADERS	:= $(THRIFT_SOURCES:.thrift=_types.h) $(THRIFT_SOURCES:.thrift=_constants.h) $(THRIFT_SOURCES:.thrift=.h)
OBJECTS		:= $(CXX_SOURCES:.cpp=.o) $(GEN_SOURCES:.cpp=.o)
DEPENDS		:= $(OBJECTS:.o=.d)
TARGET		:= src/clang-cache
DOCTARGETS	:= $(DOC_SOURCES:.1.md=.1)

VPATH		:= $(dir $(lastword $(MAKEFILE_LIST)))

default: $(TARGET)

all: $(TARGET) $(DOCTARGETS)

$(CXX_SOURCES:.cpp=.o): | $(GEN_SOURCES) $(GEN_HEADERS)

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

%.cpp %.h %_types.cpp %_types.h %_constants.cpp %_constants.h: %.thrift
	@mkdir -p $(dir $@)
	thrift --gen cpp:pure_enums -out $(dir $@) $<
	# Yes this is lame, but we don't have much choice...
	thrift --gen py:new_style -out $(dir $(lastword $(MAKEFILE_LIST)))src/vim/plugin $<

%.1: %.1.md
	@mkdir -p $(dir $@)
	ronn --roff --pipe $< >$@

.PHONY: all install clean distclean
.SECONDARY:

-include $(DEPENDS)
