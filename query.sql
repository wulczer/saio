explain select * from
	tenk1 a1,
	tenk1 b1,
	tenk1 c1,
	tenk1 d1,
	tenk1 e1,
	tenk1 f1
where
	a1.thousand = c1.thousand and b1.thousand = c1.thousand and d1.thousand = c1.thousand and e1.thousand = c1.thousand and f1.thousand = c1.thousand;
