OBJS = netstat.o library.o
CC = gcc

my_netstat: ${OBJS}
	${CC} ${OBJS} -o $@
	@echo "Applying permissions..."
	chmod 555 Makefile
	chmod 555 my_netstat

clean:
	-rm -f *.o core *.core

.c.o:
	${CC} -c $<

