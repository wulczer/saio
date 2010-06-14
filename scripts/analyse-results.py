#!/usr/bin/python

import sys

statuses = {
    '0': 'SAIO_MOVE_OK',
    '1': 'SAIO_MOVE_FAILED_FAST',
    '2': 'SAIO_MOVE_FAILED',
    '3': 'SAIO_MOVE_DISCARDED',
    '4': 'SAIO_MOVE_IMPOSSIBLE'
}

freqs = {}

for line in sys.stdin:
    num, cost, temp, result, jonrels = line.split()
    freqs.setdefault(statuses[result], 0)
    freqs[statuses[result]] += 1

print freqs
