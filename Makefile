server: server/redisserver.c \
		tcpsocket/soket.c \
		protocol/resp.c \
		ds/commands.c \
		ds/hashtable.c \
		ds/linkedlist.c \
		ds/utilities.c
		
	gcc -o  redis_server \
            ds/utilities.c \
            ds/linkedlist.c \
            ds/hashtable.c \
            ds/commands.c \
            protocol/resp.c \
            tcpsocket/soket.c \
            server/redisserver.c -levent
            