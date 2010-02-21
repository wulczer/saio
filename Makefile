MODULE_big = saio
OBJS = saio_main.o saio_join.o saio.o

PG_CONFIG = pg_config
PG_CPPFLAGS = -Werror

ifneq (,$(findstring --enable-cassert,$(shell $(PG_CONFIG) --configure)))
	PG_CPPFLAGS += -DSAIO_DEBUG
	OBJS += saio_debug.o
endif

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
