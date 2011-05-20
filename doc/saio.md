Don't use it
============

It's very bad still. Read the code, load it into your database at your
own risk. Actually, read it at your own risk too.

GUCs
====

The GUCs the module defines are:

saio
----

A boolean that enables or disables using SAIO for planning. If true, *all* queries will be planned with SAIO, if false *none*. Defaults to true.

saio\_seed
---------

A floating point seed for the random numbers generator.

saio\_equilibrium\_factor
-----------------------

Scaling factor for the query size, determining the number of loops before
equilibrium is reached.

saio\_initial\_temperature\_factor
-------------------------------

Factor determining the initial temperature of the system.

saio\_temperature\_reduction\_factor
----------------------------------

Factor determining how much the temperature is reduced each time equilibrium is
reached.

saio\_moves\_before\_frozen
------------------------

How many moves without a state change are considered a freezing condition.

saio\_algorithm
---------------

The algorithm to use when generating consecutive moves. Available algorithms
are:
 * move
 * pivot
 * recalc
