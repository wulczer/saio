EXTENSION    = saio
EXTVERSION   = $(shell grep default_version $(EXTENSION).control | \
               sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")

DOCS         = $(wildcard doc/*.txt)
TESTS        = $(wildcard test/sql/*.sql)
REGRESS      = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test
PG_CONFIG    = pg_config

MODULE_big = saio
OBJS = \
	src/saio_main.o src/saio_util.o src/saio_trees.o \
	src/saio_move.o src/saio_pivot.o src/saio_recalc.o src/saio.o src/saio_debug.o

# if the server is compiled with assertions, add the debug flag
ifneq (,$(findstring --enable-cassert,$(shell $(PG_CONFIG) --configure)))
	PG_CPPFLAGS += -DSAIO_DEBUG
endif

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
