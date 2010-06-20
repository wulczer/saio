#!/usr/bin/python

import fileinput

statuses = {
    '0': 'SAIO_MOVE_OK',
    '1': 'SAIO_MOVE_FAILED_FAST',
    '2': 'SAIO_MOVE_FAILED',
    '3': 'SAIO_MOVE_DISCARDED',
    '4': 'SAIO_MOVE_IMPOSSIBLE'
}

freqs = dict((status, {'count': 0, 'joinrels': 0}) for status in statuses.values())

start_cost = 0
min_cost = 2**31
last_name = None

for line in fileinput.input():
    if last_name and last_name != fileinput.filename():
        print "start cost: %.2f, min cost: %.2f" % (start_cost, min_cost)
        start_cost = 0
        min_cost = 2**31
    last_name = fileinput.filename()
    num, temp, cost, result, joinrels = line.split()
    start_cost = start_cost or float(cost)
    min_cost = min(min_cost, float(cost))
    freqs[statuses[result]]['count'] += 1
    freqs[statuses[result]]['joinrels'] += int(joinrels)

print "start cost: %.2f, min cost: %.2f" % (start_cost, min_cost)
print freqs
