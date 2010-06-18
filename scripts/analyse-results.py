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

for line in fileinput.input():
    num, cost, temp, result, joinrels = line.split()
    freqs[statuses[result]]['count'] += 1
    freqs[statuses[result]]['joinrels'] += int(joinrels)

print freqs
