SAIO
----

An extension module for PostgreSQL that implements join order search
with Simulated Annealing.

It's in early stages of development and definitely not suitable for production
use. It doesn't meet any portability or code quality requirements yet. It does
produce valid plans, though.

To use it, you will need the PostgreSQL development headers. Compile
and install with:

    $ make
    $ sudo make install

After that log in to your PostgreSQL server with a superuser account
and issue:

    =# LOAD 'saio';

By default all queries will be planned using SAIO, to disable it use:

    =# SET saio TO 'false';

Beware, if the module has been compiled against a server with assertion
checking enabled, it will run extremely slowly and it will write debugging
information to the `/tmp` directory.

You can learn more about SAIO by reading the PGCon2010
presentation. Get it by entering the pgcon2010/ directory and typing
`make`.
