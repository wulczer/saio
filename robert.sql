load 'saio';
set saio_algorithm TO recalc;
set saio_equilibrium_factor to 8;
set saio_initial_temperature_factor to 2.0;
set saio_temperature_reduction_factor to 0.8;
set saio_moves_before_frozen to 2;
set join_collapse_limit to 100;
set from_collapse_limit to 100;

explain select * from foo_view order by name;
