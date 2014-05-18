# Sources
DAEMON_SOURCES	:= src/daemon/client/client.cpp		\
		   src/daemon/server/cache.cpp		\
		   src/daemon/server/clc_service.cpp	\
		   src/daemon/server/server.cpp		\
		   src/daemon/utils/abspath.cpp		\
		   src/daemon/utils/cwd.cpp		\
		   src/daemon/utils/daemon.cpp		\
		   src/daemon/utils/logger.cpp		\
		   src/daemon/utils/mkdirp.cpp		\
		   src/daemon/utils/waitdir.cpp		\
		   src/daemon/clang-cache.cpp
VIM_SOURCES	:= src/vim/clc.py src/vim/clc.vim
THRIFT_SOURCES	:= src/thrift/clc_if.thrift
DOC_SOURCES	:= doc/clang-cache.1.md

# Out of tree build stuff
REPO_ROOT	:= $(dir $(lastword $(MAKEFILE_LIST)))
VPATH		:= $(REPO_ROOT)

# Compilers and flags
CXX		?= g++
CXXFLAGS	:= -std=c++11 -pedantic -Wall -Wextra -Werror	\
		   -I$(REPO_ROOT)src/daemon -Isrc/thrift	\
		   `pkg-config --cflags thrift`
LDFLAGS		:= `pkg-config --libs thrift` -lclang
PREFIX		:= /usr/local

# Intermediates and targets
DAEMON_THRIFT	:= $(THRIFT_SOURCES:.thrift=_types.cpp)		\
		   $(THRIFT_SOURCES:.thrift=_constants.cpp)	\
		   $(THRIFT_SOURCES:.thrift=.cpp)
DAEMON_THRIFT_H	:= $(DAEMON_THRIFT:.cpp=.h)
DAEMON_OBJECTS	:= $(DAEMON_SOURCES:.cpp=.o) $(DAEMON_THRIFT:.cpp=.o)
DAEMON_DEPENDS	:= $(DAEMON_OBJECTS:.o=.d)
DAEMON_TARGET	:= src/daemon/clang-cache

VIM_THRIFT	:= $(THRIFT_SOURCES:.thrift=/)
VIM_TARGET	:= src/vim/plugin

DOC_TARGET	:= $(DOC_SOURCES:.1.md=.1)

# What to build?

default: $(DAEMON_TARGET)

all: $(DAEMON_TARGET) $(VIM_TARGET) $(DOC_TARGET)

# Daemon rules

$(DAEMON_TARGET): $(DAEMON_OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(DAEMON_SOURCES:.cpp=.o): | $(DAEMON_THRIFT) $(DAEMON_THRIFT_H)

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -o $@ -c $<

%.cpp %.h %_types.cpp %_types.h %_constants.cpp %_constants.h: %.thrift
	@mkdir -p $(dir $@)
	thrift --gen cpp:pure_enums -out $(dir $@) $<
	rm -f $(dir $@)$(basename $(notdir $<))_server.skeleton.cpp

# Vim plugin rules

$(VIM_TARGET): $(VIM_SOURCES) $(VIM_THRIFT)
	@mkdir -p $@
	cp -r $^ $@

%/: %.thrift
	@mkdir -p $@
	thrift --gen py:new_style -out $@.. $<
	rm -f $@../__init__.py $@$(basename $(notdir $<))-remote

# Documentation rules

%.1: %.1.md
	@mkdir -p $(dir $@)
	ronn --roff --pipe $< >$@

# Boilerplate rules

install: all
	install -d $(PREFIX)/bin/
	install -d $(PREFIX)/share/man/man1/
	install -s -t $(PREFIX)/bin/ $(DAEMON_TARGET)
	install -t $(PREFIX)/share/man/man1/ $(DOC_TARGET)

clean:
	rm -fr $(DAEMON_OBJECTS)
	rm -fr $(DAEMON_DEPENDS)
	rm -fr $(DAEMON_THRIFT) $(DAEMON_THRIFT_H)
	rm -fr $(VIM_THRIFT)

distclean: clean
	rm -fr $(DAEMON_TARGET)
	rm -fr $(VIM_TARGET)
	rm -fr $(DOC_TARGET)

.PHONY: default all install clean distclean
.SECONDARY:

-include $(DEPENDS)
