explain select * from
	tenk1 t1,
	tenk1 t2,
	tenk1 center,
	tenk1 t3,
	tenk1 t4,
	tenk1 t5
where
	t1.thousand = center.thousand and t2.twothousand = center.twothousand and t3.fivethous = center.fivethous and t4.tenthous = center.tenthous and t5.hundred = center.hundred;
