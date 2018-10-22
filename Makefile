cc = $(CC)
cc ?= g++
WARNINGS = -Wall -Wformat -Wno-unused-command-line-argument -Wno-deprecated-declarations -Wno-unused
CFLAGS = -std=c++11 -lreadline -lc -I./include


objs = $(srcs:.cc=.o)
includes = $(wildcard include/*.hh)

exe = wulf

SRCDIR = src
OBJDIR = build

SOURCEFILES := $(addsuffix /*,$(shell find $(SRCDIR) -type d))
CODEFILES := $(wildcard $(SOURCEFILES))
SRCFILES := $(filter %.cc,$(CODEFILES))
OBJFILES := $(subst $(SRCDIR),$(OBJDIR),$(SRCFILES:%.cc=%.o))
# add the lex file to the object file targets
OBJFILES += build/lex.yy.o


.PHONY: all clean gen

# by default, compile the program with all threads :>
default:
	@$(MAKE) -j $(shell getconf _NPROCESSORS_ONLN) all

all: $(OBJDIR) $(exe)

$(OBJDIR):
	@mkdir -p $@

$(OBJDIR)/%.o: $(addprefix $(SRCDIR)/,%.cc) ${includes}
	@printf " CC\t$@\n"
	@mkdir -p $(dir $@)
	@$(cc) $(WARNINGS) $(CFLAGS) -c $< -o $@

$(exe): $(OBJFILES)
	@printf " LD\t$@\n"
	@$(cc) $(WARNINGS) -o $@ $(foreach i,$^,$(i) ) $(LDLIBS)

clean:
	@rm -rf $(exe)
	@rm -rf src/lex.yy.cc
	@rm -rf $(OBJDIR)/*.o
	@rm -rf $(OBJDIR)/render/*.o

gen: src/lex.yy.cc

src/lex.yy.cc: src/wulf.l
	@printf " FLEX\t$<\n"
	@flex -o $@ $<
