load 'saio';

set saio_seed to 0.5;
set saio_algorithm to recalc;

explain (costs off) select * from
	tenk1 t1,
	tenk1 t2,
	tenk1 t3
where
	t1.thousand = t3.thousand and
	t3.hundred = t2.hundred;

set saio_algorithm TO move;

explain (costs off) select * from
	tenk1 t1,
	tenk1 t2,
	tenk1 t3
where
	t1.thousand = t3.thousand and
	t3.hundred = t2.hundred;

set saio_algorithm TO pivot;

explain (costs off) select * from
	tenk1 t1,
	tenk1 t2,
	tenk1 t3
where
	t1.thousand = t3.thousand and
	t3.hundred = t2.hundred;
