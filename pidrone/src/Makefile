CC := g++

EXTRAINCDIRS += -I "../inc"
EXTRALDFLAGS += -L/usr/local/lib -L"../lib"
EXTRALIBS += -lwiringPi -lm -lmosquitto -lpthread -lwiringPi

CFLAGS += -O2 -W -Wall

MYOBJS = ../obj/Sonar.o ../obj/nmea.o ../obj/gps.o ../obj/serial.o

.PHONY: all	
all: ../obj/Sonar.o ../obj/nmea.o ../obj/gps.o ../obj/serial.o bin/main

.PHONY: clean
clean:
	rm -rf bin/*.o bin/* rm -rf ../obj/*.o

../obj/%.o : %.cxx 
	${CC} -c $< ${CFLAGS} -o $@ ${EXTRALDFLAGS} ${EXTRALIBS} ${EXTRAINCDIRS} 

../obj/gps.o : gps.c
	${CC} -c $< ${CFLAGS} -o $@ ${EXTRALDFLAGS} ${EXTRALIBS} ${EXTRAINCDIRS} 

../obj/serial.o : serial.c
	${CC} -c $< ${CFLAGS} -o $@ ${EXTRALDFLAGS} ${EXTRALIBS} ${EXTRAINCDIRS} 

../obj/nmea.o : nmea.c
	${CC} -c $< ${CFLAGS} -o $@ ${EXTRALDFLAGS} ${EXTRALIBS} ${EXTRAINCDIRS} 


bin/main : main.cxx
	${CC} ${CFLAGS} $^ -o $@ ${EXTRALDFLAGS} ${EXTRALIBS} ${EXTRAINCDIRS} ${MYOBJS}

