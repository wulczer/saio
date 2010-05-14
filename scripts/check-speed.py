#!/usr/bin/python

import psycopg2
from xml.dom import minidom
import time
import numpy


def check(db, query, saio, saio_params):

    conn = psycopg2.connect(host="localhost", database=db)
    conn.set_isolation_level(0)
    cur = conn.cursor()

    if saio:
        eq_factor, initial_temp, temp_reduction, before_frozen = saio_params
        cur.execute("load 'saio'")
        cur.execute("set saio_algorithm to recalc")
        cur.execute("set saio_equilibrium_factor to %d" % int(eq_factor))
        cur.execute("set saio_initial_temperature_factor to %f" % float(initial_temp))
        cur.execute("set saio_temperature_reduction_factor to %f" % float(temp_reduction))
        cur.execute("set saio_moves_before_frozen to %d" % int(before_frozen))

    SQL = ("set join_collapse_limit to 100;",
           "set from_collapse_limit to 100;")
    map(cur.execute, SQL)

    t1 = time.time()
    cur.execute(query)
    t2 = time.time()

    dom = minidom.parseString(cur.fetchone()[0])

    cost = "".join([n.data for n in
                    dom.getElementsByTagName(
                "Plan").item(0).getElementsByTagName(
                "Total-Cost").item(0).childNodes])
    t = t2 - t1
    return cost, t


LOOPS = 1

TEMPLATE = r"""      %s & %s & %s & %f & %.5f \\ \hline"""

def transpose(*args):
    a, b = zip(*[check(*args) for _ in range(LOOPS)])
    return numpy.array(map(float, a)), numpy.array(map(float, b))


#db, query = "robert", "explain (format xml) select * from foo_view order by name"
db, query = "complex", file("/home/wulczer/saio/queries/explain.sql").read()


for saio_params in (
    None,
    (4, 2.0, 0.6, 2),
    (6, 2.0, 0.6, 2),
    (8, 2.0, 0.7, 2),
    (12, 2.0, 0.4, 2),
    ):
    if not saio_params:
        costs, times = transpose(
            db, query, False, None)
        print TEMPLATE % ("GEQO", "n/a", "n/a",
                          numpy.average(costs), numpy.average(times))
    else:
        costs, times = transpose(
            db, query, True, saio_params)
        print TEMPLATE % ("SAIO", str(saio_params[0]), str(saio_params[2]),
                          numpy.average(costs), numpy.average(times))
