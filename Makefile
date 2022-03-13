CC = g++
override CFLAGS+=-Wall -g -Wno-write-strings
LIBS = -lreadline

CPP_FILES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(CPP_FILES))

#----
objetivos: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
	rm *.d

recomp: clean objetivos
	cp objetivos release/objetivos

%.o: %.cpp
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(shell find . -name "*.d" 2> /dev/null)

.PHONY: clean
clean:
	rm -f *.o *.d objetivos