load 'saio';

set join_collapse_limit to 100;
set from_collapse_limit to 100;

set saio_seed to 0.5;
set saio_algorithm to recalc;
set saio_equilibrium_factor to 1;
set saio_initial_temperature_factor to 2.0;
set saio_temperature_reduction_factor to 0.7;
set saio_moves_before_frozen to 2;
