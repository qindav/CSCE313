# makefile

all: dataserver client

FIFOreqchannel.o: reqchannel.h FIFOreqchannel.h FIFOreqchannel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c FIFOreqchannel.cpp

MQreqchannel.o: MQreqchannel.h MQreqchannel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c MQreqchannel.cpp	
	
SHMreqchannel.o: SHMreqchannel.h SHMreqchannel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c SHMreqchannel.cpp		

SHMBB.o: SHMBB.h SHMBB.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c SHMBB.cpp	
	
BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c BoundedBuffer.cpp

Histogram.o: Histogram.h Histogram.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c Histogram.cpp


dataserver: dataserver.cpp FIFOreqchannel.o MQreqchannel.o SHMreqchannel.o SHMBB.o
	g++ -g -w -Wall -O1 -std=c++11 -o dataserver dataserver.cpp FIFOreqchannel.o MQreqchannel.o SHMreqchannel.o SHMBB.o  -lpthread -lrt

client: client.cpp FIFOreqchannel.o  MQreqchannel.o SHMreqchannel.o BoundedBuffer.o Histogram.o SHMBB.o
	g++ -g -w -Wall -O1 -std=c++11 -o client client.cpp FIFOreqchannel.o MQreqchannel.o SHMreqchannel.o SHMBB.o BoundedBuffer.o Histogram.o -lpthread -lrt

clean:
	rm -rf *.o fifo* dataserver client
