explain select * from
	tenk1 t1,
	tenk1 t2,
	tenk1 t3
where
	t1.thousand = t3.thousand and
	t3.hundred = t2.hundred;
