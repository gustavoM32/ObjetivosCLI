CC = g++
CFLAGS = -Wall -g -Wno-write-strings
LIBS = -lreadline

#----

objetivos: objectives.cpp objectives.hpp calendar.o help.o io.o period.o task.o todo.o util.o
	$(CC) $(CFLAGS) objectives.cpp calendar.o help.o io.o period.o task.o todo.o util.o -o objetivos $(LIBS)

calendar.o: calendar.cpp calendar.hpp
	$(CC) $(CFLAGS) -c calendar.cpp

help.o: help.cpp help.hpp
	$(CC) $(CFLAGS) -c help.cpp

io.o: io.cpp io.hpp
	$(CC) $(CFLAGS) -c io.cpp

period.o: period.cpp period.hpp
	$(CC) $(CFLAGS) -c period.cpp

task.o: task.cpp task.hpp
	$(CC) $(CFLAGS) -c task.cpp

todo.o: todo.cpp todo.hpp
	$(CC) $(CFLAGS) -c todo.cpp

util.o: util.cpp util.hpp
	$(CC) $(CFLAGS) -c util.cpp


clear:
	rm *.o objetivos