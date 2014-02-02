clean:
	rm -rf *.o *.so *.exe

lib:
	g++ -std=gnu++11 -c -Wall -Werror -fpic -I./include src/*.cpp
	g++ -std=gnu++11 -shared -o libctxpp.so *.o
	rm -f *.o
	
lib-dbg:
	g++ -std=gnu++11 -g -c -Wall -Werror -fpic -I./include src/*.cpp
	g++ -std=gnu++11 -g -shared -o libctxpp.so *.o
	rm -f *.o

test_yield:
	g++ -std=gnu++11 -g -I./include -L./ -Wl,-rpath=. -o test_yield.exe tests/test_yield.cpp -lctxpp
	
test_delay:
	g++ -std=gnu++11 -g -I./include -L./ -Wl,-rpath=. -o test_delay.exe tests/test_delay.cpp -lctxpp
	
test_events:
	g++ -std=gnu++11 -g -I./include -L./ -Wl,-rpath=. -o test_events.exe tests/test_events.cpp -lctxpp
	
test_all_of:
	g++ -std=gnu++11 -g -I./include -L./ -Wl,-rpath=. -o test_all_of.exe tests/test_all_of.cpp -lctxpp

test_first_of:
	g++ -std=gnu++11 -g -I./include -L./ -Wl,-rpath=. -o test_first_of.exe tests/test_first_of.cpp -lctxpp
	
test_any_of:
	g++ -std=gnu++11 -g -I./include -L./ -Wl,-rpath=. -o test_any_of.exe tests/test_any_of.cpp -lctxpp