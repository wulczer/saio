MODULE_big = saio
OBJS = saio_main.o saio_debug.o saio.o

PG_CONFIG = pg_config
PG_CPPFLAGS = -Werror

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
