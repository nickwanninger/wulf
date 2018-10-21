CC = clang++
WARNINGS = -Wall -Wformat -Wno-deprecated-declarations -Wno-unused -Wdeprecated-register
CFLAGS = -I./include

objs = $(srcs:.cc=.o)
includes = $(wildcard include/*.hh)
exe = wulfc

SRCDIR = src
OBJDIR = build

STRUCTURE := $(shell find $(SRCDIR) -type d)
CODEFILES := $(addsuffix /*,$(STRUCTURE))
CODEFILES := $(wildcard $(CODEFILES))

SRCFILES := $(filter %.cc,$(CODEFILES))
OBJFILES := $(subst $(SRCDIR),$(OBJDIR),$(SRCFILES:%.cc=%.o))

.PHONY: all clean gen

all: gen $(OBJDIR) $(exe)

$(OBJDIR):
	@mkdir -p $@

$(OBJDIR)/%.o: $(addprefix $(SRCDIR)/,%.cc) ${includes}
	@printf "Compiling    $@ <- $<\n"
	@mkdir -p $(dir $@)
	@$(CC) $(WARNINGS) $(CFLAGS) -c $< -o $@

$(exe):  $(OBJFILES)
	@printf "Linking      $@ <- $(OBJDIR)/*\n"
	@$(CC) $(WARNINGS) -o $@ $(foreach i,$^,$(i) ) $(LDLIBS)

clean:
	@rm -rf $(exe)
	@rm -rf src/gen
	@rm -rf $(OBJDIR)/*.o
	@rm -rf $(OBJDIR)/render/*.o



gen: src/lex.yy.cc


src/lex.yy.cc: src/wulf.l
	flex -o $@ $<
