.PHONY : all make test run val clean

PROJ = ipk-l4-scan
LOGIN = xhubin04
SERVER = merlin.fit.vutbr.cz
CPP = gcc
OBJ = obj
CPPFLAGS = -Wall -Wextra -pedantic -std=gnu99 -o2
DEBUG = -g 
SRCS = src/%.c
OBJS = main.o scanner.o	
INCS = inc/*.h

all: $(PROJ)

%.o: $(SRCS) $(INCS)
	$(CPP) $(CPPFLAGS) $< -c -o $@

$(PROJ): $(OBJS)
	$(CPP) $(CPFLAGS) $(OBJS) -o $@ 

run: all
	./$(PROJ) 

$(LOGIN).zip: $(PROJ) src inc test Makefile LICENSE CHANGELOG.md README.md
	zip -r $@ $^

upload: $(LOGIN).zip
	scp $(LOGIN).zip $(LOGIN)@$(SERVER):~/

test: all
	./$(PROJ) -i enp7s0f3u1u1c2 -w 1000 -t 20,80,443,1023,8080 45.33.32.156

clean: 
	rm *.o ./$(PROJ) ./testfile

valgrind: $(PROJ)
	valgrind ./$(PROJ) --leakcheck=full
