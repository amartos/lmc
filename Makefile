###############################################################################
# Project infos
###############################################################################

LICENSEFILE	= LICENSE
LICENSE 	= License GPLv3
BRIEF		= LMC -- Little Man Computer
COPYRIGHT	= Copyright 2023 Alexandre Martos <contact@amartos.fr>
NAME		= $(firstword $(BRIEF))
PROJECT 	= $(shell echo $(NAME) | tr "[:upper:]" "[:lower:]")
VERSION		= $(shell find . -type f -name "$(PROJECT).[h|c]" | xargs grep version | awk '{print $$NF}')
LOGO		=


###############################################################################
# Environment
###############################################################################

SHELL		= /usr/bin/env bash

SRCS		= src
INCLUDES	= include
SUBM		= Sccroll

TESTS		= tests
UNITS		:= $(TESTS)/units
ASSETS		:= $(TESTS)/assets
TLOGS		:= $(ASSETS)/logs

SCRIPTS		= scripts
INFO	 	:= $(SCRIPTS)/pinfo
PDOC		:= $(SCRIPTS)/pdoc.awk
PCOV		:= $(SCRIPTS)/pcov.sh

BUILD		= build
BIN			:= $(BUILD)/bin
LIBS		:= $(BUILD)/lib
SUBMLIBS	:= $(SUBM:%=%/$(BUILD)/lib)
DEPS		:= $(BUILD)/deps
OBJS		:= $(BUILD)/objs
LOGS		:= $(BUILD)/logs
REPORTS		:= $(BUILD)/reports

PREFIX		?= ~/.local
INSTALL		:= $(BIN:$(BUILD)/%=$(PREFIX)/%)


###############################################################################
# Sources
###############################################################################

SRCTREE		:= $(shell find $(SRCS) -type d)
UNITREE		:= $(shell find $(UNITS) -type d)
HDRTREE		:= $(shell find $(INCLUDES) -type d)
CPPTREE		:= $(SRCTREE) $(UNITREE)

vpath %.h    $(HDRTREE)
vpath %.c    $(CPPTREE)
vpath %.log  $(LOGS)
vpath %.y    $(SRCTREE)
vpath %.l    $(SRCTREE)
vpath %.gcno $(BUILD)


###############################################################################
# Compilation parameters
###############################################################################

YDEPS		:= $(shell find $(SRCS) -type f -name "*.y")
LDEPS		:= $(shell find $(SRCS) -type f -name "*.l")
# YDEPS and LDEPS order is important for the flex/bison headers generation.
CDEPS		:= $(YDEPS:%.y=$(notdir %.tab.c)) \
				$(LDEPS:%.l=$(notdir %.yy.c)) \
				$(shell find $(SRCS) -type f -name "*.c" -and -not -name "$(PROJECT).c")
UDEPS		:= $(shell find $(UNITS) -type f -name "*.c")

CC			= gcc
CFLAGS		:= $(shell cat compile_flags.txt)
DFLAGS		= -MMD -MP -MF
LDLIBS	 	=


###############################################################################
# Code coverage
###############################################################################

COVFILE		:= $(REPORTS)/coverage
COVXML		:= $(COVFILE).xml
COVHTML		:= $(COVFILE).html

# Accepted limits for coverage in %
COVHIGH		= 98
COVLOW		= 75

COV			= gcovr
COVEXCL		:= --exclude-directories "$(TESTS)" \
				-e ".*\.(tab|yy)\.c" \
				-e ".*\.l"
COVOPTS		:= -r $(SRCS) -u $(COVEXCL)
COVOPTSXML	:= --xml-pretty --xml $(COVXML)
COVOPTSHTML	:= --html-details $(COVHTML) \
				--html-medium-threshold $(COVLOW) \
				--html-high-threshold $(COVHIGH) \
				--html-title "$(NAME) code coverage report"


###############################################################################
# Documentation
###############################################################################

DOCS		= docs
DOCSLANG	= English
DOX			= doxygen
DOXCONF		:= $(DOCS)/$(DOX).conf
DOXOPTS		:= -q $(DOXCONF)
EXAMPLES	:= $(DOCS)/examples
HTML		:= $(DOCS)/html
LATEX		:= $(DOCS)/latex
PDF			:= $(LATEX)/refman.pdf


###############################################################################
# Patterns recipes
###############################################################################

%.tab.c: %.y
	@mkdir -p $(@D)
	@bison -d -o $@ $<

%.yy.c: %.l
	@mkdir -p $(@D)
	@flex -o $@ $<

$(OBJS)/%.o: %.c
	@mkdir -p $(@D) $(DEPS)/$(*D)
	@$(CC) $(CFLAGS) $(DFLAGS) $(DEPS)/$*.d -c $< -o $@

$(BIN)/%: $(OBJS)/%.o $(CDEPS:%.c=$(OBJS)/%.o)
	@mkdir -p $(@D)
	@$(CC) $(LDLIBS) $^ -o $@

$(LOGS)/%.log: $(BIN)/%
	@mkdir -p $(@D)
	@LD_LIBRARY_PATH=$(LIBS)$(SUBMLIBS:%=:%):/usr/local/lib $< $(ARGS) &> $@ \
		&& $(INFO) pass $(*F) \
		|| ($(INFO) fail $(*F); true)

# This recipe is used when building a unit test and the test log is
# not yet available.
$(TLOGS)/%.log:
	@mkdir -p $(@D)
	@touch $@

$(LOGS)/%.difflog: $(TLOGS)/%.log $(LOGS)/%.log
	@mkdir -p $(@D)
	@git diff --no-index $^ > $@ \
		|| (sed -i "s+$(BUILD)+$(ASSETS)+g" $@ \
			&& $(INFO) error $(*F) log; true)
	@rm $(BIN)/$* $(LOGS)/$*.log


###############################################################################
# Other recipes
###############################################################################

.PHONY: all $(PROJECT) install tests docs init help
.PRECIOUS: $(DEPS)/%.d $(OBJS)/%.o $(LOGS)/%.difflog $(TLOGS)/%.log

# @brief Compile the software
all: $(PROJECT)

$(PROJECT): CDEPS  += $(shell find $(SRCS) -type f -name $(PROJECT).c)
$(PROJECT): CFLAGS += -O3
$(PROJECT): %: clean init $(BIN)/%
	@find $(BUILD) -empty -delete
	@find $(SRCS) \( -name "*.tab.c" -or -name "*.tab.h" -or -name "*.yy.c" \) -delete
	@$(INFO) ok $@

# @brief Compile the debug version of the software
debug: CFLAGS += -g -DDEBUG
debug: $(PROJECT)
	@$(INFO) ok $@

# @brief Compile and install the software
install: $(PROJECT)
	@mkdir -p $(INSTALL)
	@cp $(PROJECT) $(INSTALL)/
	@$(INFO) $(PROJECT) installed in $(INSTALL)

# @brief Execute the tests without the microcodes
tests-no-ucodes: CFLAGS = $(shell grep -v "D_UCODES" compile_flags.txt)
tests-no-ucodes: tests

# @brief Execute the tests: units tests, coverage
tests: CFLAGS += $(SUBM:%=-I%/include) -g -O0 --coverage
tests: LDLIBS += -L$(LIBS) $(SUBMLIBS:%= -L%) -lsccroll -ldl --coverage
tests: clean testsinit $(UDEPS:%.c=$(LOGS)/%.difflog)
	@$(COV) $(COVOPTS) $(COVOPTSXML) $(COVOPTSHTML) $(BUILD)
	@find $(BUILD) \( -name "*.gcno" -or -name "*.gcda" -or -empty \) -delete
	@find $(SRCS) \( -name "*.tab.c" -or -name "*.tab.h" -or -name "*.yy.c" \) -delete
	@$(INFO) ok coverage
	@$(PCOV) $(COVXML)

# Initialize any artifact needed for the tests
testsinit: init subminit
	@make -sC Sccroll > /dev/null

export NAME VERSION BRIEF LOGO DOCS EXAMPLES DOCSLANG SRC INCLUDES TESTS

# @brief Build the project documentation
docs: $(DOXCONF)
	@$(DOX) $(DOXOPTS)
	@$(MAKE) -C $(LATEX) pdf && mv $(PDF) $(DOCS)/
	@$(INFO) ok $@

# @brief Initialize the compilation directory
init:
	@mkdir -p $(BIN) $(OBJS) $(LOGS) $(DEPS) $(LIBS) $(REPORTS)

# @brief Initialize and update the submodules
subminit:
	@git submodule --quiet sync --recursive
	@git submodule --quiet update --init --recursive

# @brief Nuke all files not in VCS
clean:
	@git clean -q -d -f

# @brief Print the Makefile documentation
help:
	@echo '$(BRIEF)'
	@echo '$(COPYRIGHT)'
	@echo '$(LICENSE)'
	@echo -e "\nAvailable recipes:\n"
	@$(PDOC) Makefile | sed "s/\$$(PROJECT)/$(PROJECT)/g"

-include $(wildcard $(DEPS)/*/*.d)
