#!/usr/bin/python

import psycopg2
import optparse
from xml.dom import minidom
import time
import numpy
import sys

tests1 = [
 (0, 0, 0, 0),
 (4, 2, 0.6, 2),
 (6, 2, 0.4, 2),
 (6, 2, 0.6, 2),
 (8, 2, 0.4, 2),
 (12, 2, 0.6, 2),
 (12, 2, 0.8, 2)
]

tests2 = [(i, 2, j, 2)
          for i in range(2, 13, 1)
          for j in numpy.linspace(0.2, 0.9, 11)]

def check_time(conn, query, saio_params):

    cur = conn.cursor()

    if saio_params[0]:
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
    cur.close()
    return float(cost), float(t)

def max_memory(pid):
    for line in file('/proc/%d/status' % pid):
        if not line.startswith('VmPeak'):
            continue
        print line.split()
        num, unit = line.split()[1:3]
        return int(num)


def output(path, params, cost, t, memory):
    if params[0]:
        p = "(%d, %.3f, %.3f, %d)" % params
    else:
        p = "GEQO"
    print "Ran tests for %s, got %.5f %.5f %.5f" % (p, cost, t, memory)
    file(path, 'a').write("%d\t%f\t%f\t%d\t%f\t%f\t%f\n" % (params + (cost, t, memory)))


def run_tests(tests, query, path, loops):
    for params in tests:
        costs, times, memory = [], [], []
        for i in range(loops):
            conn = psycopg2.connect(host="localhost")
            conn.set_isolation_level(0)
            pid = conn.get_backend_pid()
            cost, t = check_time(conn, query, params)
            memory.append(max_memory(pid))
            costs.append(cost)
            times.append(t)
            conn.close()
        output(path, params, numpy.average(costs),
               numpy.average(times), numpy.average(memory))


def main():
    parser = optparse.OptionParser()
    parser.add_option("-l", "--loops", type=int)
    opts, args = parser.parse_args(sys.argv)

    tests(tests1, QUERY1, "query1.tests1.out", opts.loops)
    tests(tests2, QUERY1, "query1.tests2.out", 1)

    tests(tests1, QUERY2, "query2.tests1.out", opts.loops)
    tests(tests2, QUERY2, "query2.tests2.out", 1)
