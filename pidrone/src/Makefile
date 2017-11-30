CC := g++

EXTRAINCDIRS += -I "../inc"
EXTRALDFLAGS += -L/usr/local/lib -L"../lib"
EXTRALIBS += -lRTIMULib -lwiringPi -lm -lmosquitto -lwiringPiPca9685 -lpthread -lwiringPi

CFLAGS += -O2

MYOBJS = ../lib/Sonar.o ../lib/nmea.o ../lib/gps.o ../lib/serial.o

.PHONY: all	
all: ../lib/Sonar.o ../lib/nmea.o ../lib/gps.o ../lib/serial.o bin/sonartest bin/imutest bin/ledtest

.PHONY: clean
clean:
	rm -rf bin/*.o bin/* rm -rf ../lib/*.o

../lib/%.o : ../src/%.cxx 
	${CC} -c $< ${CFLAGS} -o $@ ${EXTRALDFLAGS} ${EXTRALIBS} ${EXTRAINCDIRS} 

../lib/gps.o : ../src/gps.c
	${CC} -c $< ${CFLAGS} -o $@ ${EXTRALDFLAGS} ${EXTRALIBS} ${EXTRAINCDIRS} 

../lib/serial.o : ../src/serial.c
	${CC} -c $< ${CFLAGS} -o $@ ${EXTRALDFLAGS} ${EXTRALIBS} ${EXTRAINCDIRS} 

../lib/nmea.o : ../src/nmea.c
	${CC} -c $< ${CFLAGS} -o $@ ${EXTRALDFLAGS} ${EXTRALIBS} ${EXTRAINCDIRS} 

bin/imutest : imutest.cxx
	${CC} ${CFLAGS} $^ -o $@ ${EXTRALDFLAGS} ${EXTRALIBS} ${EXTRAINCDIRS} ${MYOBJS}

bin/sonartest : sonartest.cxx
	${CC} ${CFLAGS} $^ -o $@ ${EXTRALDFLAGS} ${EXTRALIBS} ${EXTRAINCDIRS} ${MYOBJS}

bin/ledtest : ledtest.cxx
	${CC} ${CFLAGS} $^ -o $@ ${EXTRALDFLAGS} ${EXTRALIBS} ${EXTRAINCDIRS} ${MYOBJS}

