# Exoredis

Exoredis is an learning effort to mimic [Redis](http://redis.io). It supports the following subset of
redis commands.

1. PING
2. GET
3. SET
4. DEL
5. GETBIT
6. SETBIT
7. ZADD
8. ZCARD
9. ZCOUNT
10. ZRANGE
11. SAVE

## Setup and Run

### Dependencies

Exoredis uses libevent for asynchronous IO to serve multiple clients together. You can follow
following points to install libevent on test machine

1. Check if libevent is already installed: `$>whereis libevent`.
2. If the command prints libevet's path on terminal, it is already installed. If nothing prints out on terminal, follow following steps
    1. `$> wget https://github.com/libevent/libevent/releases/download/release-2.0.22-stable/libevent-2.0.22-stable.tar.gz`

    2. `$> tar xzf libevent-2.0.22-stable.tar.gz`
    3. `$> cd libevent-2.0.22-stable`
    4. `$> ./configure`
    5. `$> make`
    6. `$> sudo make install`
3. Add library path in your ENV variable `$> export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib/;`

### Installation
1. Download the source code of exoredis from github and put it into a directory.
2. Compile source code with make: `$> make -B` OR
3. If make utility is not installed, run gcc command as: `$> gcc -o  exoredis ds/utilities.c ds/linkedlist.c ds/hashtable.c ds/bitmap.c ds/skiplist.c ds/commands.c server/protocol.c server/redisserver.c -levent`
4. Run exoredis with file path to rdb file: `$> ./exoredis /path/to/rdb_file/file.rdb`
OR
5. Run exoredis without file path: `$> ./exoredis`
6. If ruby is installed on test machine you can run tests as: `$> cd tests/` and `$> ruby test.rb`

Exoredis expects a single argument, path to the rdb file where it is supposed to dump the DB. 
If no file path argument is given, it saves the db in current folder in **data.rdb** file.

## Redis Vs Exoredis

Exoredis speaks [RESP Protocol](http://redis.io/topics/protocol) fluently, just like redis.
So it is compatible with clients written for redis.
I have tested it with redis clients written for ruby, python and nodejs. A reason for redis's
lightning fast speed is its RESP protocol which allows it to parse incoming byte sequences in an
efficient way. However it supports space separated strings too for compatibility with telnet client.
Exoredis, just like redis, smartly handles how to parse incoming query so you can use it with telnet
and with any other redis client library too in which case it will be more fast.

A simple benchmark for redis and exoredis (using ruby) gives us following numbers. Each of the following
commands were executed for 50,000 times (except SAVE) with different arguments. SAVE command is
triggered last to save current snapshot to rdb file.

**NOTE:** Time mentioned in below table includes overhead of client.

S No | Command | Redis(seconds) | Exoredis(seconds)
-----|---------|----------------|--------------------
1 | SET | 4.7 | 5.1
2 | GET | 4.8 | 5.3
3 | SETBIT | 5.4 | 5.8
4 | GETBIT | 4.6 | 4.9
5 | ZADD | 5.0 | 5.5
6 | ZCARD | 0.0003 | 0.0003
7 | ZCOUNT | 4.7 | 5.3
8 | ZRANGE | 39.2 | 46.8
9 | SAVE | 1.7 | 3.2

## Limitations
1. Decimal precision **Redis: 16** and **Exoredis: 6**
2. No memory compression in Exoredis, so if you create bitmap with farthest bit( at position 4294967295) being set to 1 it will consume 512mb of memory. It is similar to Redis (refer [SETBIT](http://redis.io/commands/setbit) ). However, Redis compresses the memory while writing the DB on disk, while Exoredis
does not compress the memory. DB file will consume 512mb space on disk too.
3. If you find any bug :neutral_face: please let me know :grin: