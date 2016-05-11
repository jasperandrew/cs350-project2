make: lab7.cpp lab7.h classes.cpp
	g++ -g -std=c++11 lab7.cpp -o lab7

test: test.cpp lab7.h classes.cpp
	g++ -std=c++11 test.cpp -o test

clean:
	rm *.o *~  lab7 testy

del:
	rm -rf DRIVE *.dSYM