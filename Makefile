# Compile Sleeping TA Program
all:	TA

TA:	TA.o
	g++ TA.o -o TA

TA.o:
	g++ -c TA.cpp

clean:
	rm -rf *.o TA