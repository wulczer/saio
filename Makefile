MODULE_big = saio
OBJS = saio_main.o saio.o

PG_CONFIG = pg_config
PG_CPPFLAGS = -Werror -Wno-unused-function -Wno-format

ifneq (,$(findstring --enable-cassert,$(shell $(PG_CONFIG) --configure)))
	PG_CPPFLAGS += -DSAIO_DEBUG
	OBJS += saio_debug.o
endif

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
