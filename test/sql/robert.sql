load 'saio';

set saio_seed to 0.5;
set saio_algorithm TO recalc;
set saio_equilibrium_factor to 4;
set saio_initial_temperature_factor to 2.0;
set saio_temperature_reduction_factor to 0.4;
set saio_moves_before_frozen to 2;

prepare foo as select * from foo_view;
deallocate foo;
