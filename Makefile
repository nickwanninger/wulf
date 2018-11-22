CC = clang
CXX = clang++
WARNINGS = -Wall -Wformat -Wno-unused-command-line-argument -Wno-deprecated-declarations -Wno-unused
CFLAGS = -I./include -g -O3
CXXLDLIBS = -std=c++11 -pthread -lc -lgc -lreadline -g -O3

objs = $(srcs:.cc=.o)
includes = $(wildcard include/*.hh)

exe = wulf

SRCDIR = src
OBJDIR = build

SOURCEFILES := $(addsuffix /*,$(shell find $(SRCDIR) -type d))
CODEFILES := $(wildcard $(SOURCEFILES))

CXXSRCFILES := $(filter %.cc,$(CODEFILES))
CXXOBJFILES := $(subst $(SRCDIR),$(OBJDIR)/cc,$(CXXSRCFILES:%.cc=%.o))
CXXOBJFILES += build/cc/lex.yy.o


CSRCFILES := $(filter %.c,$(CODEFILES))
COBJFILES := $(subst $(SRCDIR),$(OBJDIR)/c,$(CSRCFILES:%.c=%.o))


.PHONY: all clean gen lib

# by default, compile the program with all threads :>
default:
	@$(MAKE) -j $(shell getconf _NPROCESSORS_ONLN) all

all: lib $(OBJDIR) $(exe)

$(OBJDIR):
	@mkdir -p $@

$(OBJDIR)/c/%.o: $(addprefix $(SRCDIR)/,%.c) ${includes}
	@printf " CC\t$@\n"
	@mkdir -p $(dir $@)
	@$(CC) $(WARNINGS) $(CFLAGS) -c $< -o $@


$(OBJDIR)/cc/%.o: $(addprefix $(SRCDIR)/,%.cc) ${includes}
	@printf " CXX\t$@\n"
	@mkdir -p $(dir $@)
	@$(CXX) $(WARNINGS) $(CFLAGS) -c $< -o $@

$(exe): $(CXXOBJFILES) $(COBJFILES)
	@printf " LD\t$@\n"
	@$(CXX) $(CXXLDLIBS) $(WARNINGS) -o $@ $(foreach i,$^,$(i) )

clean:
	@rm -rf $(OBJDIR)
	@rm -rf $(exe)
	@rm -rf src/lex.yy.cc
	@rm -rf lib/stdbind.so

install:
	install wulf /usr/local/bin

gen: src/lex.yy.cc

src/lex.yy.cc: src/wulf.l
	@printf " FLEX\t$<\n"
	@flex -o $@ $<


lib/stdbind.so: bindings/*.cc
	@printf " SO\t$@\n"
	@clang++ -Iinclude -shared -undefined dynamic_lookup -O3 -o lib/stdbind.so bindings/*


lib: lib/runtime.wl lib/stdbind.so
	@mkdir -p /usr/local/lib/wulf
	@cp -a lib/ /usr/local/lib/wulf
