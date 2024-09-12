
# Gabi's Makefile v2.1

# ---------------- Configuration -----------------------------------------------

## Name of the project. This will be the name of the executable placed in the
## executable directory.
PROJECT_NAME = basilisk
## Submodules the project depends on. The script will assume the subprojects
## have an 'inc/' directory object files can be pulled from after running
## make -C in the submodule.
PROJECT_SUBMODULES = unstandard
## Root source directory. Contains the c implementation files.
SRC_DIR = src
## Root include directory. Contains the c header files. Passed with -I to
## the compiler.
INC_DIR = inc $(PROJECT_SUBMODULES)/inc
## Build diectory. Will contain object and binary files linked in the final
## executable
OBJ_DIR = build
OBJ_LIB_DIR = $(PROJECT_SUBMODULES)/build
## Executable directory. Contains the final binary file.
EXC_DIR = bin
## Doxyfile location directory
DOC_CONFIG_DIR = doc
## documentation output directory, must be coherent with the doxyfile's output directory
DOC_DIR = doc-gen

## compiler
CC = gcc-13
## archiver
AR = ar

## compilation flags
CFLAGS += -Wextra -Wconversion -Wdangling-pointer -Wparentheses -Wpedantic -g --std=c2x
CFLAGS += -Werror
CFLAGS += -lc
## archiver flags
ARFLAGS = rvcs

# additional flags for defines
DFLAGS += -D_POSIX_C_SOURCE=199309L

# --------------- Internal variables -------------------------------------------

## absolute path to the library
LIBRARY = $(EXC_DIR)/lib$(PROJECT_NAME).a
## list of all c files without their path
SRC := $(notdir $(shell find $(SRC_DIR) -name *.c))
## list of all duplicate c files to enforce uniqueness of filenames
DUPL_SRC := $(strip $(shell echo $(SRC) | tr ' ' '\n' | sort | uniq -d))
## list of all target object files with their path
OBJ := $(addprefix $(OBJ_DIR)/, $(patsubst %.c, %.o, $(SRC))) $(shell find $(OBJ_LIB_DIR)/ -name *.o)
## list of all duplicate resource files to enforce uniqueness of filenames
DUPL_RES := $(strip $(shell echo $(RES) | tr ' ' '\n' | sort | uniq -d))

## makefile-managed directories
BUILD_DIRS = $(EXC_DIR) $(OBJ_DIR) $(DOC_DIR)

## additional compilation option for includes
ARGS_INCL = $(addprefix -I, $(INC_DIR))

# --------------- Make directories searching -----------------------------------

## where to find c files : all unique directories in SRC_DIR which contain a c
## file
vpath %.c $(sort $(dir $(shell find $(SRC_DIR) -name *.c)))

# --------------- Rules --------------------------------------------------------

.PHONY: all check clean count_lines documentation $(PROJECT_SUBMODULES)

# -------- compilation -----------------

all: check $(PROJECT_SUBMODULES) $(BUILD_DIRS) $(LIBRARY) | count_lines

$(LIBRARY): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/%.o: %.c
	$(CC) -c $? -o $@ $(ARGS_INCL) $(CFLAGS) $(DFLAGS)

$(PROJECT_SUBMODULES):
	make -C $@

# -------- dir spawning ----------------

$(BUILD_DIRS):
	mkdir -p $@

# -------- check -----------------------

check:
ifdef DUPL_SRC
	$(error duplicate source filenames: $(DUPL_SRC))
endif
ifdef DUPL_RES
	$(error duplicate resource filenames: $(DUPL_RES))
endif

# -------- cleaning --------------------

clean:
	rm -Rf $(BUILD_DIRS)

# -------- luxury ----------------------

count_lines:
ifneq (, $(shell which cloc))
	@cloc --hide-rate --quiet $(SRC_DIR) $(INC_DIR)
endif

documentation:
	doxygen $(DOC_CONFIG_DIR)/Doxyfile

# ----  ----  ----  ----  ----  ----

 # Senteurs d'été doux
 # Claquement brefs des touches
 # Segmentation fault
