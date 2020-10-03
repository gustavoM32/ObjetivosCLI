CC = gcc
CFLAGS = 
LIBS = -lreadline

#----

objetivos: objectives.c objectives.h calendar.o help.o io.o period.o task.o todo.o util.o
	$(CC) $(CFLAGS) objectives.c calendar.o help.o io.o period.o task.o todo.o util.o -o objetivos $(LIBS)

calendar.o: calendar.c calendar.h
	$(CC) $(CFLAGS) -c calendar.c

help.o: help.c help.h
	$(CC) $(CFLAGS) -c help.c

io.o: io.c io.h
	$(CC) $(CFLAGS) -c io.c

period.o: period.c period.h
	$(CC) $(CFLAGS) -c period.c

task.o: task.c task.h
	$(CC) $(CFLAGS) -c task.c

todo.o: todo.c todo.h
	$(CC) $(CFLAGS) -c todo.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c


clear:
	rm *.o objetivos