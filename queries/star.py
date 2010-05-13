#!/usr/bin/python

def star(n):
    sql = ''

    sql += 'drop table if exists center;\n'
    sql += 'create table center ('
    sql += ', '.join(("col%d int" % i for i in range(n)))
    sql += ');\n'

    sql += '\n'.join(('drop table if exists arm%d; create table arm%d (col '
                      'int);' % (i, i) for i in range(n)))
    sql += '\n'

    sql += '\n'.join(("insert into arm%d (select generate_series(1, 10000));"
                      % i for i in range(n)))
    sql += '\n'

    sql += 'insert into center('
    sql += ', '.join(("col%d" % i for i in range(n)))
    sql += ') values ('
    sql += ', '.join(("%d" % i for i in range(n)))
    sql += ');\n'

    sql += 'analyze;\n'

    sql += 'explain select * from '
    sql += ', '.join(("arm%d" % i for i in range(n / 2)))
    sql += ', center, '
    sql += ', '.join(("arm%d" % i for i in range(n / 2, n)))
    sql += ' where '
    sql += ' and '.join(('arm%d.col = col%d' % (i, i) for i in range(n)))
    sql += ';'

    return sql

if __name__ == "__main__":
    import sys
    print star(int(sys.argv[1]))
