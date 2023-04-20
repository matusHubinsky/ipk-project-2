
# @file list.c
# @author Matus Hubinsky
# @brief data structure list and functions for it
# @date 17.04.2023
# @version 1.0.1

.PHONY : all make test run upload snapshot zip clean

PROJ = ipk-l4-scan
LOGIN = xhubin04
SERVER = merlin.fit.vutbr.cz
CPP = gcc
CPPFLAGS = -Wall -Wextra -pedantic -std=gnu99 
DEBUG = -g 
SRCS = src/%.c
OBJS = arg.o list.o udp.o main.o 
INCS = inc/*.h

all: $(PROJ)

%.o: $(SRCS) $(INCS)
	$(CPP) $(CPPFLAGS) $< -c -o $@

$(PROJ): $(OBJS)
	$(CPP) $(CPFLAGS) $(OBJS) -lpcap -o $@ 

$(LOGIN).zip: $(PROJ) html src inc test Makefile LICENSE CHANGELOG.md README.md 
	zip -r $@ $^

upload: $(LOGIN).zip
	scp $(LOGIN).zip $(LOGIN)@$(SERVER):~/

run: all
	./$(PROJ) -i enp7s0f3u1u1c2 -w 5000 -u 22,68,80,123,443,8080 scanme.nmap.org

test: all
	./$(PROJ) -i wlp3s0 -w 1000 -u 22,68,80,123 scanme.nmap.org

snapshot:
	git archive HEAD --format=zip > $(LOGIN).zip

clean: 
	rm *.o ./$(PROJ) ./testfile

valgrind: $(PROJ)
	valgrind ./$(PROJ) --leakcheck=full
