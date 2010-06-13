#!/usr/bin/python

import sys

freqs = {}

for line in sys.stdin:
    num, cost, temp, result, jonrels = line.split()
    freqs.setdefault(result, 0)
    freqs[result] += 1

print freqs
