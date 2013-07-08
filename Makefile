CXXFLAGS = -std=c++0x -DGETOPTPP_DEBUG -I.

demo: getopt++.o demo.cc
	g++ ${CXXFLAGS} -o demo demo.cc getopt++.o

getopt++.o: getopt++.cc
	g++ ${CXXFLAGS} -c -o getopt++.o getopt++.cc

getopt++.a: getopt++.o
	ar -q getopt++.a getopt++.o
	ranlib getopt++.a

getopt++.so: getopt++.cc
	g++ ${CXXFLAGS} -fPIC -shared -o getopt++.so getopt++.cc

clean:
	rm -f demo *.o *.a *.so
