provider saio {
    probe planning_start();
    probe planning_done();

    /* param: status, current cost as integer, current temperature, elapsed loops, failed moves */
    probe step_done(int, int, int, int, int);
}
