CC := gcc
SRCD := src
#TSTD := tests
BLDD := build
BIND := bin
INCD := include

ALL_SRCF := $(shell find $(SRCD) -type f -name *.c)
ALL_OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(ALL_SRCF:.c=.o))
FUNC_FILES := $(filter-out build/main.o, $(ALL_OBJF))

#TEST_SRC := $(shell find $(TSTD) -type f -name *.c)

INC := -I $(INCD)

CFLAGS := -Wall -Werror
COLORF := -DCOLOR
DFLAGS := -g -DDEBUG
PRINT_STAMENTS := -DERROR -DSUCCESS -DWARN -DINFO
CURL := -lcurl

STD := -std=gnu11
#TEST_LIB := -lcriterion
LIBS :=

CFLAGS += $(STD)

EXEC := antv
#TEST_EXEC := $(EXEC)_tests


.PHONY: clean all

all: setup $(EXEC) #$(TEST_EXEC)

debug: CFLAGS += $(DFLAGS) $(PRINT_STAMENTS) $(COLORF)
debug: all

setup:
	mkdir -p bin build

$(EXEC): $(ALL_OBJF)
	$(CC) $^ $(CURL) -o $(BIND)/$@

#$(TEST_EXEC): $(FUNC_FILES)
#	$(CC) $(CFLAGS) $(INC) $(FUNC_FILES) $(TEST_SRC) $(TEST_LIB) -o $(BIND)/$(TEST_EXEC)
	#$(CC) $(CFLAGS) $(INC) $(FUNC_FILES) $(TEST_SRC) $(TEST_LIB) -o $(BIND)/$(TEST_EXEC)
$(BLDD)/%.o: $(SRCD)/%.c
	$(CC) $(CFLAGS) $(INC) -c  -o $@ $<

clean:
	rm -rf $(BLDD) $(BIND)