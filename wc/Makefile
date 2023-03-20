# Tell make these are not real files
.PHONY = all clean debug test

# by default make all three executables 
# (added echo to force/finish output coloring)
all: main debug tests
	@echo ""

debug: main-debug

# runs unit tests
test: tests
	./tests

#### Commands and flags used
MKDIR_P = mkdir -p

CC = clang

LDFLAGS = -pthread -lm
DEBUG_FLAGS := -g -O0

# Generate dependencies
GENDEPFLAGS = -MT $@ -MMD -MP -MF $(DEPENDENCIESDIR)/$(notdir $*).d

# cute way to highlight output with background colors
#SET_GREEN = @echo -n "\e[42m"
#SET_DEFAULT =	@echo -n "\e[49m"
#### (end) commands and flags

### four subdirectories for intermediate products
# all .d (dependency) files live here
DEPENDENCIESDIR = deps
# normal .o files
OBJDIR    = obj
# .o files unique to tests
TSTOBJDIR = obj.test
# .o files compiled with DEBUG_FLAGS
DBGOBJDIR = obj.debug

# Directories for generated files
DIRS = ${DEPENDENCIESDIR} ${OBJDIR} ${TSTOBJDIR} ${DBGOBJDIR}

$(DIRS):
	${MKDIR_P} $@
#### (end dirs)

#### compilation rules
# how to compile normal (not debug) objects
$(OBJDIR)/%.o $(TSTOBJDIR)/%.o : %.c | $(DIRS)
	$(CC) $(CFLAGS) $(GENDEPFLAGS) -c $< -o $@  
$(TSTOBJDIR)/munit.o : munit/munit.c | $(DIRS)
	$(CC) $(CFLAGS) $(GENDEPFLAGS) -c $< -o $@  

# how to compile objects suitable for debugger/debugging
$(DBGOBJDIR)/%.o : %.c | $(DIRS)
	$(CC) $(CFLAGS) $(GENDEPFLAGS) $(DEBUG_FLAGS) -c $< -o $@  
#### (end) compilation rules

### List of objects generated from list of .c files
# All C sources ONLY in current directory
# (Hack this if you're using subdirs)
SRC = $(wildcard *.c)

# two special sources (have a main() function; for tests)
MAIN_SRC = main.c
# if any test-specific C files, add them here
TEST_SRC = tests.c

# all sources except those that have a main() or are specific to tests
COMMON_SRC = $(filter-out $(MAIN_SRC) $(TEST_SRC),$(SRC))

#### "groups" of objects
MAIN_OBJ      = $(MAIN_SRC:%.c=$(OBJDIR)/%.o)
TEST_OBJ      = $(TEST_SRC:%.c=$(TSTOBJDIR)/%.o) $(TSTOBJDIR)/munit.o
COMMON_OBJ    = $(COMMON_SRC:%.c=$(OBJDIR)/%.o)
COMMON_DBGOBJ = $(COMMON_SRC:%.c=$(DBGOBJDIR)/%.o)

#### all objects that each target needs
ALL_OBJ_FOR_MAIN  = $(COMMON_OBJ)    $(MAIN_OBJ)
ALL_OBJ_FOR_TESTS = $(COMMON_OBJ)    $(TEST_OBJ)
ALL_OBJ_FOR_DEBUG = $(COMMON_DBGOBJ) $(DBGOBJDIR)/main.o

main :  $(ALL_OBJ_FOR_MAIN)
	$(SET_GREEN)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) | xargs echo -n 
	$(SET_DEFAULT)

main-debug : $(ALL_OBJ_FOR_DEBUG) 
	$(SET_GREEN)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $@ $^ $(LDFLAGS) | xargs echo -n 
	$(SET_DEFAULT)

tests :  $(ALL_OBJ_FOR_TESTS)
	$(SET_GREEN)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) | head -c -1 
	$(SET_DEFAULT)

clean:
	rm -f main tests main-debug \
		$(OBJDIR)/*.o \
		$(DBGOBJDIR)/*.o \
		$(TSTOBJDIR)/*.o \
		$(DEPENDENCIESDIR)/*.d
	rmdir --ignore-fail-on-non-empty $(DIRS)


-include $(SRC:%.c=$(DEPENDENCIESDIR)/%.d)


# Makefile debug stuff - uncomment these to see commands and line numbers
# OLD_SHELL := $(SHELL)
# SHELL = $(warning Building $@)$(OLD_SHELL)

# v1.2
