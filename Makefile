CC = clang++
WARNINGS = -Wall -Wformat -Wno-deprecated-declarations -Wno-unused
CFLAGS = -I./include

objs = $(srcs:.cpp=.o)
exe = wulfc

SRCDIR = src
OBJDIR = build

STRUCTURE := $(shell find $(SRCDIR) -type d)
CODEFILES := $(addsuffix /*,$(STRUCTURE))
CODEFILES := $(wildcard $(CODEFILES))

SRCFILES := $(filter %.cpp,$(CODEFILES))
OBJFILES := $(subst $(SRCDIR),$(OBJDIR),$(SRCFILES:%.cpp=%.o))

.PHONY: all clean

all: $(OBJDIR) $(exe)

$(OBJDIR):
	@mkdir -p $@

$(OBJDIR)/%.o: $(addprefix $(SRCDIR)/,%.cpp)
	@printf "Compiling    $@ <- $<\n"
	@mkdir -p $(dir $@)
	@$(CC) $(WARNINGS) $(CFLAGS) -c $< -o $@

$(exe):  $(OBJFILES)
	@printf "Linking      $@ <- $(OBJDIR)/*\n"
	@$(CC) $(WARNINGS) -o $@ $(foreach i,$^,$(i) ) $(LDLIBS)

clean:
	@rm -rf $(exe)
	@rm -rf $(OBJDIR)/*.o
	@rm -rf $(OBJDIR)/render/*.o

