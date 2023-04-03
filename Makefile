.PHONY : all make test run val clean

PROJ = ipk-l4-scan
LOGIN = xhubin04
SERVER = merlin.fit.vutbr.cz
CPP = gcc
OBJ = obj
CPPFLAGS = -Wall
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
	./$(PROJ) -i eth0 -w 10 -t 22,631,8080 localhost

clean: 
	rm *.o ./$(PROJ) ./testfile

valgrind: $(PROJ)
	valgrind ./$(PROJ) --leakcheck=full