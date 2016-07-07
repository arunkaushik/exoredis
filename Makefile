server:
	gcc -o	redis_server \
			ds/commands.c \
            ds/utilities.c \
            ds/linkedlist.c \
            ds/hashtable.c \