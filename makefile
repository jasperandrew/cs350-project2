make: del lab7.cpp lab7.h classes.cpp
	g++ -g -std=c++11 lab7.cpp -o lab7

clean:
	rm *.o *~ lab7

del:
	rm -rf DRIVE *.dSYM