###############################################################################
# Informations sur le projet
###############################################################################

LICENSEFILE	= LICENSE
LICENSE 	= $(shell head -n 2 $(LICENSEFILE) | tail -n 1)
BRIEF		= $(shell head -n 1 $(LICENSEFILE))
NAME		= $(firstword $(BRIEF))
PROJECT 	= $(shell echo $(NAME) | tr "[:upper:]" "[:lower:]")
VERSION		= $(shell find . -type f -name "$(PROJECT).[h|c]" | xargs grep version | awk '{print $$NF}')
LOGO		=


###############################################################################
# Environnement
###############################################################################

SHELL		= /usr/bin/env bash

SRCS		= src
INCLUDES	= include

TESTS		= tests
UNITS		:= $(TESTS)/units
ASSETS		:= $(TESTS)/assets
TLOGS		:= $(ASSETS)/logs

SCRIPTS		= scripts
MOCKS		:= $(SCRIPTS)/mocks.awk
INFO	 	:= $(SCRIPTS)/pinfo
PDOC		:= $(SCRIPTS)/pdoc.awk

BUILD		= build
BIN			:= $(BUILD)/bin
LIBS		:= $(BUILD)/lib
DEPS		:= $(BUILD)/deps
OBJS		:= $(BUILD)/objs
LOGS		:= $(BUILD)/logs
REPORTS		:= $(BUILD)/reports

PREFIX		?= ~/.local
INSTALL		:= $(BIN:$(BUILD)/%=$(PREFIX)/%)


###############################################################################
# Chemins des sources
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


###############################################################################
# Paramètres de compilation
###############################################################################

YDEPS		:= $(shell find $(SRCS) -type f -name "*.y")
LDEPS		:= $(shell find $(SRCS) -type f -name "*.l")
# L'ordre YDEPS LDEPS est important pour la génération de fichiers en-têtes
# par bison, utilisés par flex.
CDEPS		:= $(YDEPS:%.y=$(notdir %.tab.c)) \
				$(LDEPS:%.l=$(notdir %.yy.c)) \
				$(shell find $(SRCS) -type f -name "*.c" -and -not -name "$(PROJECT).c")
UDEPS		:= $(shell find $(UNITS) -type f -name "*.c")

CC			= gcc
STD			= gnu99
CFLAGS		:= $(shell cat compile_flags.txt)
DFLAGS		= -MMD -MP -MF
LDLIBS	 	=


###############################################################################
# Paramètres de couverture de code
###############################################################################

COVFILE		:= $(REPORTS)/coverage
COVXML		:= $(COVFILE).xml
COVHTML		:= $(COVFILE).html

# Limites de couverture de code acceptées en %
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
# Paramètres de documentation
###############################################################################

DOCS		= docs
DOCSLANG	= French
DOX			= doxygen
DOXCONF		:= $(DOCS)/$(DOX).conf
DOXOPTS		:= -q $(DOXCONF)
EXAMPLES	:= $(DOCS)/examples
HTML		:= $(DOCS)/html
LATEX		:= $(DOCS)/latex
PDF			:= $(LATEX)/refman.pdf


###############################################################################
# Cibles à patterns
###############################################################################

%.tab.c: %.y
	@mkdir -p $(dir $@)
	@bison -d -o $@ $<

%.yy.c: %.l
	@mkdir -p $(dir $@)
	@flex -o $@ $<

$(OBJS)/%.o: %.c
	@mkdir -p $(dir $@) $(DEPS)/$(dir $*)
	@$(CC) $(CFLAGS) $(DFLAGS) $(DEPS)/$*.d -c $< -o $@

$(BIN)/%: $(OBJS)/%.o $(CDEPS:%.c=$(OBJS)/%.o)
	@mkdir -p $(dir $@)
	@$(CC) $(LDLIBS) $^ -o $@

$(LOGS)/%.log: $(BIN)/%
	@mkdir -p $(dir $@)
	@LD_LIBRARY_PATH=$(LIBS):/usr/local/lib $< $(ARGS) &> $@ \
		&& $(INFO) pass $(notdir $*) \
		|| ($(INFO) fail $(notdir $*); true)

$(LOGS)/%.difflog: $(TLOGS)/%.log $(LOGS)/%.log
	@mkdir -p $(dir $@)
	@git diff --no-index $^ > $@ \
		|| (sed -i "s+$(BUILD)+$(ASSETS)+g" $@ \
			&& $(INFO) error $(notdir $* log); true)
	@rm $(BIN)/$* $(LOGS)/$*.log


###############################################################################
# Autres cibles
###############################################################################

.PHONY: all $(PROJECT) install tests docs init help
.PRECIOUS: $(DEPS)/%.d $(OBJS)/%.o $(LOGS)/%.difflog

# @brief Compile la cible principale du project
all: $(PROJECT)

# Compile la cible principale du project (cible par défaut)
$(PROJECT): CDEPS  += $(shell find $(SRCS) -type f -name $(PROJECT).c)
$(PROJECT): CFLAGS += -O3
$(PROJECT): %: clean init $(BIN)/%
	@find $(BUILD) -empty -delete
	@find $(SRCS) \( -name "*.tab.c" -or -name "*.tab.h" -or -name "*.yy.c" \) -delete
	@$(INFO) ok $@

# @brief Compile la cible principale avec fonctionnalités de debuggage
debug: CFLAGS += -g -DDEBUG
debug: $(PROJECT)
	@$(INFO) ok $@

# @brief Installe le logiciel compilé sur le système.
install: $(PROJECT)
	@mkdir -p $(INSTALL)
	@cp $(PROJECT) $(INSTALL)/
	@$(INFO) $(PROJECT) installed in $(INSTALL)

# @brief Exécute les tests du projet (unitaires, couverture, etc...)
tests: CFLAGS += -g -O0 --coverage
tests: LDLIBS += -L$(LIBS) -lsccroll $(shell $(MOCKS) $*.c) --coverage
tests: clean init $(UDEPS:%.c=$(LOGS)/%.difflog)
	@$(COV) $(COVOPTS) $(COVOPTSXML) $(COVOPTSHTML) $(BUILD)
	@find $(BUILD) \( -name "*.gcno" -or -name "*.gcda" -or -empty \) -delete
	@find $(SRCS) \( -name "*.tab.c" -or -name "*.tab.h" -or -name "*.yy.c" \) -delete
	@$(INFO) ok coverage

export NAME VERSION BRIEF LOGO DOCS EXAMPLES DOCSLANG SRC INCLUDES TESTS

# @brief Génère la documentation automatisée du projet
docs: $(DOXCONF)
	@$(DOX) $(DOXOPTS)
	@$(MAKE) -C $(LATEX) pdf && mv $(PDF) $(DOCS)/
	@$(INFO) ok $@

# @brief Initialise le dossier de compilation
init:
	@mkdir -p $(BIN) $(OBJS) $(LOGS) $(DEPS) $(LIBS) $(REPORTS)

# @brief Nettoyage post-compilation
clean:
	@git clean -q -d -f

# @brief Affiche la documentation du Makefile
help:
	@head -n 5 $(LICENSEFILE)
	@echo "Cibles disponibles:"
	@$(PDOC) Makefile | sed "s/\$$(PROJECT)/$(PROJECT)/g"

-include $(wildcard $(DEPS)/*/*.d)
