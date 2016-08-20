server: server/redisserver.c \
		protocol/resp.c \
		ds/commands.c \
		ds/skiplist.c \
		ds/bitmap.c \
		ds/hashtable.c \
		ds/linkedlist.c \
		ds/utilities.c
        
		gcc -o  exoredis \
			ds/utilities.c \
			ds/linkedlist.c \
			ds/hashtable.c \
			ds/bitmap.c \
			ds/skiplist.c \
			ds/commands.c \
			protocol/resp.c \
			server/redisserver.c -levent
            