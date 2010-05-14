#!/usr/bin/python

import psycopg2
from xml.dom import minidom
import time
import numpy


def check(saio=False):
    conn = psycopg2.connect(host="localhost", database="robert")
    conn.set_isolation_level(0)
    cur = conn.cursor()

    if saio:
        SQL = """
load 'saio';
set saio_algorithm TO recalc;
set saio_equilibrium_factor to 6;
set saio_initial_temperature_factor to 2.0;
set saio_temperature_reduction_factor to 0.4;
set saio_moves_before_frozen to 2;
"""
        cur.execute(SQL)

    SQL = """
set join_collapse_limit to 100;
set from_collapse_limit to 100;
"""
    cur.execute(SQL)

    t1 = time.time()
    cur.execute("explain (format xml) select * from foo_view order by name")
    t2 = time.time()

    dom = minidom.parseString(cur.fetchone()[0])

    cost = "".join([n.data for n in
                    dom.getElementsByTagName(
                "Plan").item(0).getElementsByTagName(
                "Total-Cost").item(0).childNodes])
    t = t2 - t1
    return cost, t


LOOPS = 10

def transpose(saio=False):
    a, b = zip(*[check(transpose) for _ in range(LOOPS)])
    return numpy.array(map(float, a)), numpy.array(map(float, b))

geqo_costs, geqo_times = transpose()
saio_costs, saio_times = transpose()

print "GEQO:", numpy.average(geqo_costs), numpy.average(geqo_times)
print "SAIO:", numpy.average(saio_costs), numpy.average(saio_times)
