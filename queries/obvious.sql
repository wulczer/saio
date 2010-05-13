load 'saio';

set saio_moves_before_frozen to 1;
set saio_initial_temperature_factor to 0.000001;
set saio_algorithm TO recalc;

explain select * from
	tenk1 t1,
	tenk1 t2,
	tenk1 t3
where
	t1.thousand = t3.thousand and
	t3.hundred = t2.hundred;
