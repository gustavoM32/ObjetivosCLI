CC = g++
CFLAGS = -Wall -g -Wno-write-strings
LIBS = -lreadline
MODULES = calendar.o calendarUtil.o help.o io.o period.o task.o taskUtil.o todo.o util.o ioold.o

#----

objetivos: objectives.cpp objectives.hpp $(MODULES)
	$(CC) $(CFLAGS) objectives.cpp $(MODULES) -o objetivos $(LIBS)

calendar.o: calendar.cpp calendar.hpp
	$(CC) $(CFLAGS) -c calendar.cpp

calendarUtil.o: calendarUtil.cpp calendarUtil.hpp
	$(CC) $(CFLAGS) -c calendarUtil.cpp

help.o: help.cpp help.hpp
	$(CC) $(CFLAGS) -c help.cpp

io.o: io.cpp io.hpp
	$(CC) $(CFLAGS) -c io.cpp

period.o: period.cpp period.hpp
	$(CC) $(CFLAGS) -c period.cpp

task.o: task.cpp task.hpp
	$(CC) $(CFLAGS) -c task.cpp

taskUtil.o: taskUtil.cpp taskUtil.hpp
	$(CC) $(CFLAGS) -c taskUtil.cpp

todo.o: todo.cpp todo.hpp
	$(CC) $(CFLAGS) -c todo.cpp

util.o: util.cpp util.hpp
	$(CC) $(CFLAGS) -c util.cpp

ioold.o: ioold.cpp ioold.hpp
	$(CC) $(CFLAGS) -c ioold.cpp

clear:
	rm *.o objetivos