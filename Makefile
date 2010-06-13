MODULE_big = saio
OBJS = \
	saio_main.o saio_util.o saio_trees.o \
	saio_move.o saio_pivot.o saio_recalc.o saio.o saio_debug.o

PG_CONFIG = /home/wulczer/tmp/postgresql/bin/pg_config
PG_CPPFLAGS = -Werror -Wno-format -Wno-unused-function

ifneq (,$(findstring --enable-cassert,$(shell $(PG_CONFIG) --configure)))
	PG_CPPFLAGS += -DSAIO_DEBUG
endif

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

# Handrolled, using existing installation with data
# loaded from the load_data.sql file in test/sql
check:
	$(top_builddir)/src/test/regress/pg_regress \
		--use-existing \
		--inputdir=$(srcdir)/test \
		--outputdir=$(srcdir) \
		--psqldir=$(bindir) \
		--schedule $(srcdir)/test/schedule
